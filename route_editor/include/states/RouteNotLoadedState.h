#ifndef ROUTE_NOT_LOADED_STATE_H
#define ROUTE_NOT_LOADED_STATE_H

#include "states/State.h"

class RouteNotLoadedState : public State
{
public:
    RouteNotLoadedState(EditorContext& editor_context);
    virtual ~RouteNotLoadedState() override;

    virtual void fill_status_bar() const override;
};

#endif // ROUTE_NOT_LOADED_STATE_H
