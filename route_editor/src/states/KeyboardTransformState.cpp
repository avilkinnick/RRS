#include "editor/states/KeyboardTransformState.h"

#include "editor/EditorContext.h"
#include "editor/Keyboard.h"
#include "editor/RouteObject.h"
#include "editor/StateManager.h"
#include "editor/states/State.h"

#include <vsg/maths/vec3.h>
#include <vsg/ui/KeyEvent.h>

KeyboardTransformState::KeyboardTransformState(EditorContext& editor_context)
    : State(editor_context)
{
}

KeyboardTransformState::~KeyboardTransformState() = default;

void KeyboardTransformState::handle_key_press()
{
    const auto& keyboard = editor_context.keyboard;

    if (keyboard->pressed_once(vsg::KEY_Return))
    {
        confirm_transform();
    }
    else if (editor_context.keyboard->pressed_once(vsg::KEY_Escape))
    {
        cancel_transform();
    }
}

void KeyboardTransformState::handle_button_press()
{
}

void KeyboardTransformState::handle_mouse_move()
{
}

void KeyboardTransformState::set_begin_intersection(vsg::dvec3 begin_intersection)
{
    this->begin_intersection = begin_intersection;
}

void KeyboardTransformState::confirm_transform() const
{
}

void KeyboardTransformState::cancel_transform() const
{
    for (const auto& object : editor_context.selected_objects)
    {
        object->set_matrix(object->get_initial_matrix());
    }

    editor_context.state_manager->defer_switch_to(STATE_BASIC);
}
