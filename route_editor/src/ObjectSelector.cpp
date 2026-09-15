#include "editor/ObjectSelector.h"

#include "editor/Action.h"
#include "editor/Camera.h"
#include "editor/EditorContext.h"
#include "editor/Gizmo.h"
#include "editor/Keyboard.h"
#include "editor/Mouse.h"
#include "editor/Route.h"
#include "editor/RouteObject.h"
#include "editor/commands/CommandManager.h"
#include "editor/commands/DeleteObjectsCommand.h"
#include "editor/commands/PasteObjectsCommand.h"
#include "editor/commands/RotateObjectsCommand.h"
#include "editor/commands/ScaleObjectsCommand.h"
#include "editor/commands/SelectObjectsCommand.h"
#include "editor/commands/TranslateObjectsCommand.h"
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
    const auto& camera = editor_context.camera;
    const auto& selected_objects = editor_context.selected_objects;
    const auto& command_manager = editor_context.command_manager;
    const auto& window = editor_context.window;
    const auto& gizmo = editor_context.gizmo;

    if (mouse->is_rmb_pressed() || state_ != State::INITIAL ||
        selected_objects.empty())
    {
        return;
    }

    if (keyboard->pressed(ACTION_PASTE_OBJECTS))
    {
        auto command = std::make_unique<PasteObjectsCommand>(editor_context);
        command->execute();
        command_manager->push(std::move(command));
        return;
    }
    else if (keyboard->pressed(ACTION_DELETE_OBJECTS))
    {
        auto command = std::make_unique<DeleteObjectsCommand>(editor_context);
        command->execute();
        command_manager->push(std::move(command));
        return;
    }

    const bool pressed_action_move = keyboard->pressed(ACTION_TRANSLATE_OBJECTS);
    const bool pressed_action_rotate = keyboard->pressed(ACTION_ROTATE_OBJECTS);
    const bool pressed_action_scale = keyboard->pressed(ACTION_SCALE_OBJECTS);

    if (!pressed_action_move && !pressed_action_rotate && !pressed_action_scale)
    {
        return;
    }

    calculate_intersection_mouse_and_plane(mouse->get_x(), mouse->get_y(),
        window->extent2D(), camera->get_inverse_view_matrix(),
        camera->get_inverse_projection_matrix(), gizmo->get_curr_pos(),
        camera->get_front(), prev_intersect_pos_);

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

void ObjectSelector::apply([[maybe_unused]] vsg::ButtonPressEvent& buttonPress)
{
    // Обработка перенесена в commands/BasicEditorState.cpp
}

void ObjectSelector::apply(vsg::ButtonReleaseEvent& buttonRelease)
{
    editor_context.gizmo->apply(buttonRelease);
}

void ObjectSelector::apply(vsg::MoveEvent& moveEvent)
{
    const auto& mouse = editor_context.mouse;
    const auto& selected_objects = editor_context.selected_objects;
    const auto& camera = editor_context.camera;
    const auto& window = editor_context.window;
    const auto& gizmo = editor_context.gizmo;

    gizmo->apply(moveEvent);

    if (state_ == State::INITIAL)
    {
        return;
    }

    vsg::dvec3 world_intersection;
    calculate_intersection_mouse_and_plane(mouse->get_x(), mouse->get_y(),
        window->extent2D(), camera->get_inverse_view_matrix(),
        camera->get_inverse_projection_matrix(), gizmo->get_curr_pos(),
        camera->get_front(), world_intersection);

    switch (state_)
    {
        case State::KEYBOARD_GRAB:
        {
            return;
        }
        case State::KEYBOARD_ROTATE:
        {
            const vsg::dvec3& gizmo_pos = gizmo->get_curr_pos();

            if (world_intersection == gizmo_pos)
            {
                return;
            }

            const vsg::dvec3 prev_vec = vsg::normalize(prev_intersect_pos_ - gizmo_pos);
            const vsg::dvec3 curr_vec = vsg::normalize(world_intersection - gizmo_pos);

            prev_intersect_pos_ = world_intersection;

            const vsg::dvec3& camera_up = camera->get_up();

            double prev_acos = acos(vsg::dot(prev_vec, camera_up));
            double curr_acos = acos(vsg::dot(curr_vec, camera_up));

            const vsg::dvec3& front = camera->get_front();

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
            const vsg::dvec3& gizmo_pos = gizmo->get_curr_pos();

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

void ObjectSelector::confirm_keyboard_transformation()
{
    const auto& command_manager = editor_context.command_manager;
    const auto& selected_objects = editor_context.selected_objects;
    const auto& camera = editor_context.camera;
    const auto& gizmo = editor_context.gizmo;

    switch (state_)
    {
        case State::KEYBOARD_ROTATE:
        {
            auto command = std::make_unique<RotateObjectsCommand>(
                editor_context, selected_objects, gizmo->get_curr_pos(),
                camera->get_front(), total_rotation_rad_);
            command_manager->push(std::move(command));

            break;
        }
        case State::KEYBOARD_SCALE:
        {
            auto command = std::make_unique<ScaleObjectsCommand>(
                editor_context, selected_objects, gizmo->get_curr_pos(),
                total_scale_);
            command_manager->push(std::move(command));

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
