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
    const auto& window = editor_context.window;
    const auto& gizmo = editor_context.gizmo;

    if (mouse->is_rmb_pressed() || state_ != State::INITIAL ||
        selected_objects.empty())
    {
        return;
    }

    if (!keyboard->pressed(ACTION_SCALE_OBJECTS))
    {
        return;
    }

    calculate_intersection_mouse_and_plane(mouse->get_x(), mouse->get_y(),
        window->extent2D(), camera->get_inverse_view_matrix(),
        camera->get_inverse_projection_matrix(), gizmo->get_curr_pos(),
        camera->get_front(), prev_intersect_pos_);

    total_scale_ = {1.0, 1.0, 1.0};

    for (const auto& object : selected_objects)
    {
        object->save_matrix();
    }

    state_ = State::KEYBOARD_SCALE;
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
    const auto& gizmo = editor_context.gizmo;

    switch (state_)
    {
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
