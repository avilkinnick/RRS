#include "editor/states/KeyboardTranslateState.h"

#include "editor/Camera.h"
#include "editor/commands/CommandManager.h"
#include "editor/EditorContext.h"
#include "editor/Gizmo.h"
#include "editor/Keyboard.h"
#include "editor/Mouse.h"
#include "editor/RouteObject.h"
#include "editor/StateManager.h"
#include "editor/commands/TranslateObjectsCommand.h"
#include "editor/editor_math.h"

#include <memory>
#include <utility>
#include <vsg/maths/vec3.h>
#include <vsg/ui/KeyEvent.h>
#include <vsg/ui/PointerEvent.h>

KeyboardTranslateState::KeyboardTranslateState(EditorContext& editor_context)
    : State(editor_context)
{
    name = "KeyboardTranslateState";
}

KeyboardTranslateState::~KeyboardTranslateState() = default;

void KeyboardTranslateState::handle_key_press()
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

void KeyboardTranslateState::handle_button_press()
{
    const auto& mouse = editor_context.mouse;
    const auto& state_manager = editor_context.state_manager;
    const auto& selected_objects = editor_context.selected_objects;
    const auto& command_manager = editor_context.command_manager;

    switch (mouse->get_button_mask())
    {
        case vsg::BUTTON_MASK_1:
        {
            auto command = std::make_unique<TranslateObjectsCommand>(
                editor_context, selected_objects,
                prev_intersection - begin_intersection);
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

void KeyboardTranslateState::handle_mouse_move()
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

    const vsg::dvec3 translation = world_intersection - prev_intersection;
    prev_intersection = world_intersection;

    for (const auto& object : selected_objects)
    {
        object->move(translation);
    }
}

void KeyboardTranslateState::set_begin_intersection(vsg::dvec3 begin_intersection)
{
    this->begin_intersection = begin_intersection;
    prev_intersection = begin_intersection;
}
