#include "editor/states/KeyboardScaleState.h"

#include "editor/Camera.h"
#include "editor/commands/CommandManager.h"
#include "editor/EditorContext.h"
#include "editor/Gizmo.h"
#include "editor/Keyboard.h"
#include "editor/Mouse.h"
#include "editor/RouteObject.h"
#include "editor/StateManager.h"
#include "editor/commands/RotateObjectsCommand.h"
#include "editor/commands/ScaleObjectsCommand.h"
#include "editor/editor_math.h"

#include <vsg/maths/vec3.h>

KeyboardScaleState::KeyboardScaleState(EditorContext& editor_context)
    : State(editor_context)
{
    name = "KeyboardScaleState";
}

KeyboardScaleState::~KeyboardScaleState() = default;

void KeyboardScaleState::handle_key_press()
{
    const auto& keyboard = editor_context.keyboard;
    const auto& state_manager = editor_context.state_manager;
    const auto& selected_objects = editor_context.selected_objects;

    if (keyboard->pressed_once(vsg::KEY_Escape))
    {
        for (const auto& object : selected_objects)
        {
            object->set_matrix(object->get_initial_matrix());
        }

        state_manager->defer_switch_to(STATE_BASIC);
    }
}

void KeyboardScaleState::handle_button_press()
{
    const auto& mouse = editor_context.mouse;
    const auto& state_manager = editor_context.state_manager;
    const auto& selected_objects = editor_context.selected_objects;
    const auto& command_manager = editor_context.command_manager;
    const auto& gizmo = editor_context.gizmo;

    switch (mouse->get_button_mask())
    {
        case vsg::BUTTON_MASK_1:
        {
            auto command = std::make_unique<ScaleObjectsCommand>(
                editor_context, selected_objects, gizmo->get_curr_pos(),
                scale);
            command_manager->push(std::move(command));
            state_manager->defer_switch_to(STATE_BASIC);
            return;
        }
        case vsg::BUTTON_MASK_3:
        {
            for (const auto& object : selected_objects)
            {
                object->set_matrix(object->get_initial_matrix());
            }
            state_manager->defer_switch_to(STATE_BASIC);
            return;
        }
        default:
        {
            return;
        }
    }
}

void KeyboardScaleState::handle_mouse_move()
{
    const auto& mouse = editor_context.mouse;
    const auto& window = editor_context.window;
    const auto& camera = editor_context.camera;
    const auto& gizmo = editor_context.gizmo;
    const auto& selected_objects = editor_context.selected_objects;

    vsg::dvec3 world_intersection;
    calculate_intersection_mouse_and_plane(mouse->get_x(), mouse->get_y(),
        window->extent2D(), camera->get_inverse_view_matrix(),
        camera->get_inverse_projection_matrix(), gizmo->get_curr_pos(),
        camera->get_front(), world_intersection);

    const vsg::dvec3& gizmo_pos = gizmo->get_curr_pos();

    if (vsg::length(world_intersection - gizmo_pos) < 1.0e-6f)
    {
        return;
    }

    const vsg::dvec3 begin_vec = begin_intersection - gizmo_pos;
    const vsg::dvec3 curr_vec = world_intersection - gizmo_pos;

    const double scale_value = vsg::length(curr_vec) / vsg::length(begin_vec);
    scale = {scale_value, scale_value, scale_value};

    for (const auto& object : selected_objects)
    {
        object->set_matrix(object->get_initial_matrix());
        object->scale_relative_to_pivot(gizmo_pos, scale, object->matrix);
    }
}

void KeyboardScaleState::set_begin_intersection(vsg::dvec3 begin_intersection)
{
    this->begin_intersection = begin_intersection;
    scale = {1.0, 1.0, 1.0};
}
