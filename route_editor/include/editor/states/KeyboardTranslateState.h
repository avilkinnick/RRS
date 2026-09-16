#ifndef EDITOR_STATES_KEYBOARD_TRANSLATE_STATE_H
#define EDITOR_STATES_KEYBOARD_TRANSLATE_STATE_H

#include "editor/states/KeyboardTransformState.h"

#include <vsg/maths/vec3.h>

class KeyboardTranslateState : public KeyboardTransformState
{
public:
    KeyboardTranslateState(EditorContext& editor_context);
    virtual ~KeyboardTranslateState() override;

    virtual void handle_mouse_move() override;

    virtual void set_begin_intersection(vsg::dvec3 begin_intersection) override;

private:
    vsg::dvec3 prev_intersection;

private:
    virtual void confirm_transform() const override;
};

#endif // EDITOR_STATES_KEYBOARD_TRANSLATE_STATE_H
