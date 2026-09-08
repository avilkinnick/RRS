#include "states/KeyboardScaleState.h"

KeyboardScaleState::KeyboardScaleState(EditorContext& editor_context)
    : State(editor_context)
{
    name = "KeyboardScaleState";
}

KeyboardScaleState::~KeyboardScaleState() = default;

void KeyboardScaleState::handle_key_press()
{
}
