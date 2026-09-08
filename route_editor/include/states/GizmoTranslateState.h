#ifndef GIZMO_TRANSLATE_STATE_H
#define GIZMO_TRANSLATE_STATE_H

#include "states/State.h"

class GizmoTranslateState : public State
{
public:
    GizmoTranslateState(EditorContext& editor_context);
    virtual ~GizmoTranslateState() override;

    virtual void handle_key_press() override;
};

#endif // GIZMO_TRANSLATE_STATE_H
