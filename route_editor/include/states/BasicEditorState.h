#ifndef BASIC_EDITOR_STATE_H
#define BASIC_EDITOR_STATE_H

#include "states/State.h"

class CommandManager;
struct EditorContext;

class BasicEditorState : public State
{
public:
    BasicEditorState(EditorContext& editor_context, CommandManager& command_manager);
    virtual ~BasicEditorState() override;

    virtual void handle_key_press() override;

    virtual void handle_button_press() override;
    virtual void handle_mouse_scroll() override;

private:
    CommandManager& command_manager;
};

#endif // BASIC_EDITOR_STATE_H
