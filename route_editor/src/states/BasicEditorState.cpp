#include "states/BasicEditorState.h"

#include "Action.h"
#include "Camera.h"
#include "EditorContext.h"
#include "Journal.h"
#include "Keyboard.h"
#include "Mouse.h"
#include "Route.h"
#include "RouteObject.h"
#include "StateManager.h"
#include "commands/CommandManager.h"
#include "filesystem.h"

#include <vsg/ui/PointerEvent.h>
#include <vsgImGui/imgui.h>

BasicEditorState::BasicEditorState(EditorContext& editor_context)
    : State(editor_context)
{
    name = "BasicEditorState";
}

BasicEditorState::~BasicEditorState() = default;

void BasicEditorState::handle_key_press()
{
    const auto& keyboard = editor_context.keyboard;
    const auto& camera = editor_context.camera;
    const auto& command_manager = editor_context.command_manager;

    if (keyboard->pressed_once(ACTION_UNDO_COMMAND))
    {
        command_manager->undo();
    }
    else if (keyboard->pressed_once(ACTION_REDO_COMMAND))
    {
        command_manager->redo();
    }
    else if(keyboard->pressed_once(ACTION_SAVE_ROUTE))
    {
        save_route();
    }
    else
    {
        camera->handle_key_press();
    }
}

void BasicEditorState::handle_button_press()
{
    const auto& mouse = editor_context.mouse;
    const auto& state_manager = editor_context.state_manager;

    switch (mouse->get_button_mask())
    {
        case vsg::BUTTON_MASK_1:
        {
            return;
        }
        case vsg::BUTTON_MASK_3:
        {
            state_manager->defer_switch_to(STATE_NAVIGATION);
            return;
        }
        default:
        {
            return;
        }
    }
}

void BasicEditorState::handle_mouse_scroll()
{
    editor_context.camera->handle_mouse_scroll();
}

void BasicEditorState::save_route()
{
    const auto& fs = FileSystem::getInstance();
    const std::string save_dir = fs.combinePath(editor_context.route->route_dir, "topology", "map");

    try
    {
        // Создаём резервную копию
        std::filesystem::copy_file(
            fs.combinePath(save_dir, "route1.map"),
            fs.combinePath(save_dir, "route1.map.prev"),
            std::filesystem::copy_options::overwrite_existing
        );
    }
    catch (const std::filesystem::filesystem_error& e)
    {
        Journal::instance()->error(e.what());
    }

    // Перезаписываем рабочую копию
    std::ofstream route_map_file{fs.combinePath(save_dir, "route1.map")};

    editor_context.static_objects_mutex.lock();
    for (const auto& object : editor_context.static_objects)
    {
        const vsg::dvec3& translation{object->get_translation()};
        const vsg::dvec3 rotation_deg{-object->get_rotation_deg()};

        route_map_file << object->label << "," <<
            translation.x << "," << translation.y << "," << translation.z << "," <<
            rotation_deg.x << "," << rotation_deg.y << "," << rotation_deg.z << ";\n";
    }
    editor_context.static_objects_mutex.unlock();
}
