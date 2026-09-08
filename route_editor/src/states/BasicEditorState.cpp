#include "states/BasicEditorState.h"

#include "Action.h"
#include "Camera.h"
#include "EditorContext.h"
#include "Keyboard.h"
#include "Mouse.h"
#include "StateManager.h"
#include "commands/CommandManager.h"

#include <vsg/ui/PointerEvent.h>
#include <vsgImGui/imgui.h>

BasicEditorState::BasicEditorState(
    EditorContext& editor_context,
    CommandManager& command_manager
)
    : State(editor_context)
    , command_manager(command_manager)
{
    name = "BasicEditorState";
}

BasicEditorState::~BasicEditorState() = default;

void BasicEditorState::handle_key_press()
{
    const auto& keyboard = editor_context.keyboard;
    const auto& camera = editor_context.camera;

    if (keyboard->pressed_once(ACTION_UNDO_COMMAND))
    {
        command_manager.undo();
    }
    else if (keyboard->pressed_once(ACTION_REDO_COMMAND))
    {
        command_manager.redo();
    }
    else
    {
        camera->handle_key_press();
    }
}

void BasicEditorState::handle_button_press()
{
    const auto& mouse = editor_context.mouse;
    const auto& state_manager = editor_context.state_manager;

    switch (mouse->get_button_mask())
    {
        case vsg::BUTTON_MASK_1:
        {
            return;
        }
        case vsg::BUTTON_MASK_3:
        {
            state_manager->defer_switch_to(STATE_NAVIGATION);
            return;
        }
        default:
        {
            return;
        }
    }
}

void BasicEditorState::handle_mouse_scroll()
{
    editor_context.camera->handle_mouse_scroll();
}
