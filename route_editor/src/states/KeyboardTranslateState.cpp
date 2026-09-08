#include "states/KeyboardTranslateState.h"

KeyboardTranslateState::KeyboardTranslateState(EditorContext& editor_context)
    : State(editor_context)
{
    name = "KeyboardTranslateState";
}

KeyboardTranslateState::~KeyboardTranslateState() = default;

void KeyboardTranslateState::handle_key_press()
{
}
