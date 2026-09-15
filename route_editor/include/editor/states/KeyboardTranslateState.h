#ifndef KEYBOARD_TRANSLATE_STATE_H
#define KEYBOARD_TRANSLATE_STATE_H

#include "editor/states/State.h"

#include <vsg/maths/vec3.h>

class KeyboardTranslateState : public State
{
public:
    KeyboardTranslateState(EditorContext& editor_context);
    virtual ~KeyboardTranslateState() override;

    virtual void handle_key_press() override;
    virtual void handle_button_press() override;
    virtual void handle_mouse_move() override;

    void set_begin_intersection(vsg::dvec3 begin_intersection);

private:
    vsg::dvec3 begin_intersection;
    vsg::dvec3 prev_intersection;
};

#endif // KEYBOARD_TRANSLATE_STATE_H
