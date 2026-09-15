#include "editor/states/KeyboardRotateState.h"

KeyboardRotateState::KeyboardRotateState(EditorContext& editor_context)
    : State(editor_context)
{
    name = "KeyboardRotateState";
}

KeyboardRotateState::~KeyboardRotateState() = default;

void KeyboardRotateState::handle_key_press()
{
}

void KeyboardRotateState::set_begin_intersection(vsg::dvec3 begin_intersection)
{
    this->begin_intersection = begin_intersection;
}
