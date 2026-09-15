#include "editor/states/KeyboardRotateState.h"

#include "editor/Camera.h"
#include "editor/commands/CommandManager.h"
#include "editor/EditorContext.h"
#include "editor/Gizmo.h"
#include "editor/Keyboard.h"
#include "editor/Mouse.h"
#include "editor/RouteObject.h"
#include "editor/StateManager.h"
#include "editor/commands/RotateObjectsCommand.h"
#include "editor/editor_math.h"

#include <vsg/maths/vec3.h>

KeyboardRotateState::KeyboardRotateState(EditorContext& editor_context)
    : State(editor_context)
{
    name = "KeyboardRotateState";
}

KeyboardRotateState::~KeyboardRotateState() = default;

void KeyboardRotateState::handle_key_press()
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

void KeyboardRotateState::handle_button_press()
{
    const auto& mouse = editor_context.mouse;
    const auto& state_manager = editor_context.state_manager;
    const auto& selected_objects = editor_context.selected_objects;
    const auto& command_manager = editor_context.command_manager;
    const auto& gizmo = editor_context.gizmo;
    const auto& camera = editor_context.camera;

    switch (mouse->get_button_mask())
    {
        case vsg::BUTTON_MASK_1:
        {
            auto command = std::make_unique<RotateObjectsCommand>(
                editor_context, selected_objects, gizmo->get_curr_pos(),
                camera->get_front(), rotation_rad);
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

void KeyboardRotateState::handle_mouse_move()
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

    const vsg::dvec3 begin_vec = vsg::normalize(begin_intersection - gizmo_pos);
    const vsg::dvec3 curr_vec = vsg::normalize(world_intersection - gizmo_pos);

    const vsg::dvec3& camera_up = camera->get_up();

    double begin_acos = acos(vsg::dot(begin_vec, camera_up));
    double curr_acos = acos(vsg::dot(curr_vec, camera_up));

    const vsg::dvec3& front = camera->get_front();

    if (begin_vec != camera_up && begin_vec != -camera_up &&
        vsg::dot(vsg::cross(begin_vec, camera_up), front) < 0.0)
    {
        begin_acos = 2 * vsg::PI - begin_acos;
    }

    if (curr_vec != camera_up && curr_vec != -camera_up &&
        vsg::dot(vsg::cross(curr_vec, camera_up), front) < 0.0)
    {
        curr_acos = 2 * vsg::PI - curr_acos;
    }

    for (const auto& object : selected_objects)
    {
        rotation_rad = begin_acos - curr_acos;
        object->set_matrix(object->get_initial_matrix());
        object->rotate_around_pivot(gizmo_pos, front, rotation_rad, object->matrix);
    }
}

void KeyboardRotateState::set_begin_intersection(vsg::dvec3 begin_intersection)
{
    this->begin_intersection = begin_intersection;
    rotation_rad = 0.0;
}
