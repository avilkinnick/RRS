#ifndef KEYBOARD_SCALE_STATE_H
#define KEYBOARD_SCALE_STATE_H

#include "editor/states/State.h"

#include <vsg/maths/vec3.h>

class KeyboardScaleState : public State
{
public:
    KeyboardScaleState(EditorContext& editor_context);
    virtual ~KeyboardScaleState() override;

    virtual void handle_key_press() override;
    virtual void handle_button_press() override;
    virtual void handle_mouse_move() override;

    void set_begin_intersection(vsg::dvec3 begin_intersection);

private:
    vsg::dvec3 begin_intersection;
    vsg::dvec3 scale;
};

#endif // KEYBOARD_SCALE_STATE_H
