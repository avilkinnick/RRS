#include "editor/ObjectSelector.h"

#include "editor/Action.h"
#include "editor/Camera.h"
#include "editor/EditorContext.h"
#include "editor/Gizmo.h"
#include "editor/Keyboard.h"
#include "editor/Mask.h"
#include "editor/Mouse.h"
#include "editor/Route.h"
#include "editor/RouteObject.h"
#include "editor/commands/CommandManager.h"
#include "editor/commands/DeleteObjectsCommand.h"
#include "editor/commands/PasteObjectsCommand.h"
#include "editor/commands/RotateObjectsCommand.h"
#include "editor/commands/ScaleObjectsCommand.h"
#include "editor/commands/SelectObjectsCommand.h"
#include "editor/commands/TranslateObjects.h"
#include "editor/editor_math.h"

#include <vsg/core/Mask.h>
#include <vsg/maths/common.h>
#include <vsg/maths/transform.h>
#include <vsg/maths/vec3.h>
#include <vsg/nodes/Node.h>
#include <vsg/ui/KeyEvent.h>
#include <vsg/ui/PointerEvent.h>
#include <vsg/utils/LineSegmentIntersector.h>

#include <cmath>
#include <memory>
#include <utility>

ObjectSelector::ObjectSelector(EditorContext& context)
    : editor_context(context)
{
}

void ObjectSelector::apply([[maybe_unused]] vsg::KeyPressEvent& keyPress)
{
    const auto& keyboard = editor_context.keyboard;
    const auto& mouse = editor_context.mouse;
    const auto& selected_objects = editor_context.selected_objects;
    auto& copied_objects = editor_context.copied_objects;

    if (mouse->is_rmb_pressed() || state_ != State::INITIAL ||
        selected_objects.empty())
    {
        return;
    }

    if (keyboard->pressed(ACTION_COPY_OBJECTS))
    {
        copied_objects = selected_objects;
        return;
    }
    else if (keyboard->pressed(ACTION_PASTE_OBJECTS))
    {
        auto command = std::make_unique<PasteObjectsCommand>(editor_context);
        command->execute();
        editor_context.command_manager->push(std::move(command));
        return;
    }
    else if (keyboard->pressed(ACTION_DELETE_OBJECTS))
    {
        auto command = std::make_unique<DeleteObjectsCommand>(editor_context);
        command->execute();
        editor_context.command_manager->push(std::move(command));
        return;
    }

    const bool pressed_action_move = keyboard->pressed(ACTION_TRANSLATE_OBJECTS);
    const bool pressed_action_rotate = keyboard->pressed(ACTION_ROTATE_OBJECTS);
    const bool pressed_action_scale = keyboard->pressed(ACTION_SCALE_OBJECTS);

    if (!pressed_action_move && !pressed_action_rotate &&
        !pressed_action_scale)
    {
        return;
    }

    const auto& camera = editor_context.camera;

    calculate_intersection_mouse_and_plane(mouse->get_x(), mouse->get_y(),
        editor_context.window->extent2D(), camera->get_inverse_view_matrix(),
        camera->get_inverse_projection_matrix(), editor_context.gizmo->get_curr_pos(),
        camera->get_front(), prev_intersect_pos_);

    total_translation_ = {0.0, 0.0, 0.0};
    total_rotation_rad_ = 0.0;
    total_scale_ = {1.0, 1.0, 1.0};

    for (const auto& object : selected_objects)
    {
        object->save_matrix();
    }

    if (pressed_action_move)
    {
        state_ = State::KEYBOARD_GRAB;
    }
    else if (pressed_action_rotate)
    {
        state_ = State::KEYBOARD_ROTATE;
    }
    else if (pressed_action_scale)
    {
        state_ = State::KEYBOARD_SCALE;
    }
}

void ObjectSelector::apply(vsg::ButtonPressEvent& buttonPress)
{
    if (buttonPress.handled)
    {
        return;
    }

    const auto& keyboard = editor_context.keyboard;
    const auto& mouse = editor_context.mouse;
    const auto& selected_objects = editor_context.selected_objects;

    if (state_ != State::INITIAL)
    {
        if (mouse->is_lmb_pressed())
        {
            confirm_keyboard_transformation();
            return;
        }
        else if (mouse->is_rmb_pressed())
        {
            cancel_keyboard_transformation();
            return;
        }
    }

    if (!(buttonPress.button == 1 &&
          buttonPress.mask == vsg::BUTTON_MASK_1))
    {
        return;
    }

    // If we have selected objects and clicked on Gizmo,
    // handle Gizmo intersection (start moving objects with Gizmo)
    if (!selected_objects.empty() && editor_context.gizmo->handle_intersections())
    {
        return;
    }

    const auto& camera = editor_context.camera;

    const auto intersector = vsg::LineSegmentIntersector::create(*camera,
        buttonPress.x, buttonPress.y);
    if (!intersector)
    {
        return;
    }
    intersector->traversalMask = MASK_CLICKABLE;

    editor_context.scene_graph->accept(*intersector);

    auto& intersections = intersector->intersections;
    if (intersections.empty())
    {
        // If we clicked on empty space without shift
        // while there were selected objects,
        // deselect them all
        if (!selected_objects.empty() && !keyboard->get_shift_state())
        {
            auto command = std::make_unique<SelectObjectsCommand>(editor_context);
            command->objects_to_deselect = selected_objects;
            command->update_description();
            command->execute();
            editor_context.command_manager->push(std::move(command));
        }

        return;
    }

    for (const vsg::Node* const node : intersections.front()->nodePath)
    {
        if (const RouteObject* const object = node->cast<RouteObject>())
        {
            select_object(vsg::ref_ptr(const_cast<RouteObject*>(object)));
            break;
        }
    }

    intersections.clear();
}

void ObjectSelector::apply(vsg::ButtonReleaseEvent& buttonRelease)
{
    editor_context.gizmo->apply(buttonRelease);
}

void ObjectSelector::apply(vsg::MoveEvent& moveEvent)
{
    editor_context.gizmo->apply(moveEvent);

    if (state_ == State::INITIAL)
    {
        return;
    }

    const auto& mouse = editor_context.mouse;
    const auto& selected_objects = editor_context.selected_objects;

    const auto& camera = editor_context.camera;

    vsg::dvec3 world_intersection;
    calculate_intersection_mouse_and_plane(mouse->get_x(), mouse->get_y(),
        editor_context.window->extent2D(), camera->get_inverse_view_matrix(),
        camera->get_inverse_projection_matrix(), editor_context.gizmo->get_curr_pos(),
        camera->get_front(), world_intersection);

    const vsg::dvec3& camera_up = camera->get_up();

    switch (state_)
    {
        case State::KEYBOARD_GRAB:
        {
            const vsg::dvec3 translation = world_intersection - prev_intersect_pos_;

            prev_intersect_pos_ = world_intersection;
            total_translation_ += translation;

            for (const auto& object : selected_objects)
            {
                object->move(translation);
            }

            return;
        }
        case State::KEYBOARD_ROTATE:
        {
            const vsg::dvec3& gizmo_pos = editor_context.gizmo->get_curr_pos();

            if (world_intersection == gizmo_pos)
            {
                return;
            }

            const vsg::dvec3 prev_vec = vsg::normalize(prev_intersect_pos_ - gizmo_pos);
            const vsg::dvec3 curr_vec = vsg::normalize(world_intersection - gizmo_pos);

            prev_intersect_pos_ = world_intersection;

            double prev_acos = acos(vsg::dot(prev_vec, camera_up));
            double curr_acos = acos(vsg::dot(curr_vec, camera_up));

            const vsg::dvec3& front = editor_context.camera->get_front();

            if (prev_vec != camera_up && prev_vec != -camera_up &&
                vsg::dot(vsg::cross(prev_vec, camera_up), front) < 0.0)
            {
                prev_acos = 2 * vsg::PI - prev_acos;
            }

            if (curr_vec != camera_up && curr_vec != -camera_up &&
                vsg::dot(vsg::cross(curr_vec, camera_up), front) < 0.0)
            {
                curr_acos = 2 * vsg::PI - curr_acos;
            }

            for (const auto& object : selected_objects)
            {
                const double rotation_rad = prev_acos - curr_acos;
                total_rotation_rad_ += rotation_rad;

                object->rotate_around_pivot(gizmo_pos, front, rotation_rad, object->matrix);
            }

            return;
        }
        case State::KEYBOARD_SCALE:
        {
            const vsg::dvec3& gizmo_pos = editor_context.gizmo->get_curr_pos();

            if (world_intersection == gizmo_pos)
            {
                return;
            }

            const vsg::dvec3 prev_vec = prev_intersect_pos_ - gizmo_pos;
            const vsg::dvec3 curr_vec = world_intersection - gizmo_pos;

            prev_intersect_pos_ = world_intersection;

            const double scale_value = vsg::length(curr_vec) / vsg::length(prev_vec);

            const vsg::dvec3 scale = {scale_value, scale_value, scale_value};
            total_scale_ *= scale;

            for (const auto& object : selected_objects)
            {
                object->scale_relative_to_pivot(gizmo_pos, scale, object->matrix);
            }

            return;
        }
        default:
        {
            return;
        }
    }
}

void ObjectSelector::select_object(vsg::ref_ptr<RouteObject> object)
{
    auto command = std::make_unique<SelectObjectsCommand>(editor_context);

    if (editor_context.keyboard->get_shift_state())
    {
        if (object->get_is_selected())
        {
            command->objects_to_deselect.emplace_back(object);
        }
        else
        {
            command->objects_to_select.emplace_back(object);
        }
    }
    else
    {
        const RouteObjects& selected_objects = editor_context.selected_objects;

        if (selected_objects.empty())
        {
            command->objects_to_select.emplace_back(object);
        }
        else if (object->get_is_selected())
        {
            if (selected_objects.size() == 1)
            {
                command->objects_to_deselect.emplace_back(object);
            }
            else
            {
                for (const auto& selected_object : selected_objects)
                {
                    if (selected_object != object)
                    {
                        command->objects_to_deselect.emplace_back(selected_object);
                    }
                }
            }
        }
        else
        {
            command->objects_to_select.emplace_back(object);
            command->objects_to_deselect = selected_objects;
        }
    }

    command->update_description();
    command->execute();

    editor_context.command_manager->push(std::move(command));
}

void ObjectSelector::confirm_keyboard_transformation()
{
    switch (state_)
    {
        case State::KEYBOARD_GRAB:
        {
            auto command = std::make_unique<TranslateObjects>(editor_context,
                editor_context.selected_objects, total_translation_);
            editor_context.command_manager->push(std::move(command));

            break;
        }
        case State::KEYBOARD_ROTATE:
        {
            const auto& camera = editor_context.camera;

            auto command = std::make_unique<RotateObjectsCommand>(editor_context,
                editor_context.selected_objects, editor_context.gizmo->get_curr_pos(),
                camera->get_front(), total_rotation_rad_);
            editor_context.command_manager->push(std::move(command));

            break;
        }
        case State::KEYBOARD_SCALE:
        {
            auto command = std::make_unique<ScaleObjectsCommand>(editor_context,
                editor_context.selected_objects, editor_context.gizmo->get_curr_pos(), total_scale_);
            editor_context.command_manager->push(std::move(command));

            break;
        }
        default:
        {
            break;
        }
    }

    state_ = State::INITIAL;
}

void ObjectSelector::cancel_keyboard_transformation()
{
    for (const auto& object : editor_context.selected_objects)
    {
        object->set_matrix(object->get_initial_matrix());
    }

    state_ = State::INITIAL;
}
