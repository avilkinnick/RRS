#ifndef KEYBOARD_SCALE_STATE_H
#define KEYBOARD_SCALE_STATE_H

#include "editor/states/State.h"

class KeyboardScaleState : public State
{
public:
    KeyboardScaleState(EditorContext& editor_context);
    virtual ~KeyboardScaleState() override;

    virtual void handle_key_press() override;
};

#endif // KEYBOARD_SCALE_STATE_H
