#ifndef KEYBOARD_ROTATE_STATE_H
#define KEYBOARD_ROTATE_STATE_H

#include "editor/states/State.h"

class KeyboardRotateState : public State
{
public:
    KeyboardRotateState(EditorContext& editor_context);
    virtual ~KeyboardRotateState() override;

    virtual void handle_key_press() override;
};

#endif // KEYBOARD_ROTATE_STATE_H
