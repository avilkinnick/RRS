#include "EventHandler.h"

#include "StateManager.h"
#include "states/State.h"

EventHandler::EventHandler(StateManager& state_manager)
    : state_manager(state_manager)
{
}

EventHandler::~EventHandler() = default;

void EventHandler::apply([[maybe_unused]] vsg::KeyPressEvent& keyPress)
{
    state_manager.get_editor_state()->handle_key_press();
}

void EventHandler::apply([[maybe_unused]] vsg::KeyReleaseEvent& keyRelease)
{
    state_manager.get_editor_state()->handle_key_release();
}

void EventHandler::apply([[maybe_unused]] vsg::ButtonPressEvent& buttonPress)
{
    state_manager.get_editor_state()->handle_button_press();
}

void EventHandler::apply([[maybe_unused]] vsg::ButtonReleaseEvent& buttonRelease)
{
    state_manager.get_editor_state()->handle_button_release();
}


void EventHandler::apply([[maybe_unused]] vsg::MoveEvent& moveEvent)
{
    state_manager.get_editor_state()->handle_mouse_move();
}

void EventHandler::apply([[maybe_unused]] vsg::ScrollWheelEvent& scrollWheel)
{
    state_manager.get_editor_state()->handle_mouse_scroll();
}

void EventHandler::update(double delta_time)
{
    state_manager.get_editor_state()->update(delta_time);
}
