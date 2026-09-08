#ifndef GIZMO_ROTATE_STATE_H
#define GIZMO_ROTATE_STATE_H

#include "states/State.h"

class GizmoRotateState : public State
{
public:
    explicit GizmoRotateState(EditorContext& editor_context);
    virtual ~GizmoRotateState() override;

    virtual void handle_key_press() override;
};

#endif // GIZMO_ROTATE_STATE_H
