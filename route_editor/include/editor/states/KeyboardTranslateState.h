#ifndef KEYBOARD_TRANSLATE_STATE_H
#define KEYBOARD_TRANSLATE_STATE_H

#include "editor/states/State.h"
#include <vsg/maths/vec3.h>

class KeyboardTranslateState : public State
{
public:
    vsg::dvec3 begin_intersection_pos;

public:
    KeyboardTranslateState(EditorContext& editor_context);
    virtual ~KeyboardTranslateState() override;

    virtual void handle_key_press() override;
};

#endif // KEYBOARD_TRANSLATE_STATE_H
