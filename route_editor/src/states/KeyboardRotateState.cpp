#include "states/KeyboardRotateState.h"

KeyboardRotateState::KeyboardRotateState(EditorContext& editor_context)
    : State(editor_context)
{
    name = "KeyboardRotateState";
}

KeyboardRotateState::~KeyboardRotateState() = default;

void KeyboardRotateState::handle_key_press()
{
}
