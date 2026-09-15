#ifndef KEYBOARD_ROTATE_STATE_H
#define KEYBOARD_ROTATE_STATE_H

#include "editor/states/State.h"

#include <vsg/maths/vec3.h>

class KeyboardRotateState : public State
{
public:
    KeyboardRotateState(EditorContext& editor_context);
    virtual ~KeyboardRotateState() override;

    virtual void handle_key_press() override;

    void set_begin_intersection(vsg::dvec3 begin_intersection);

private:
    vsg::dvec3 begin_intersection;
};

#endif // KEYBOARD_ROTATE_STATE_H
