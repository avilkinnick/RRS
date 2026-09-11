#include "editor/commands/DeleteObjectsCommand.h"

#include "editor/commands/Command.h"
#include "editor/EditorContext.h"
#include "editor/Gizmo.h"
#include "editor/Route.h"
#include "editor/RouteObject.h"

#include <vsg/core/Mask.h>
#include <vsg/nodes/Switch.h>

#include <algorithm>
#include <cstdio>

DeleteObjectsCommand::DeleteObjectsCommand(EditorContext& editor_context)
    : Command(editor_context)
    , objects_(editor_context.selected_objects)
{
    update_description();
}

void DeleteObjectsCommand::execute()
{
    for (const auto& object : objects_)
    {
        object->deselect();

        editor_context.static_objects_mutex.lock();
        auto& static_objects = editor_context.static_objects;
        static_objects.erase(std::find(static_objects.begin(), static_objects.end(), object));
        editor_context.static_objects_mutex.unlock();

        editor_context.route->children.erase(
            std::find_if(editor_context.route->children.begin(), editor_context.route->children.end(),
                [object](const vsg::Switch::Child& child) {
                    return child.node == object;
                }
            )
        );
    }

    editor_context.compile_infos_mutex.lock();
    editor_context.compile_infos.emplace_back(CompileInfo{nullptr, editor_context.route});
    editor_context.compile_infos_mutex.unlock();

    editor_context.gizmo->update_visibility();
}

void DeleteObjectsCommand::undo()
{
    for (const auto& object : objects_)
    {
        editor_context.compile_infos_mutex.lock();
        editor_context.compile_infos.emplace_back(CompileInfo{
            editor_context.route, object, vsg::MASK_ALL});
        editor_context.compile_infos_mutex.unlock();

        editor_context.static_objects_mutex.lock();
        editor_context.static_objects.emplace_back(object);
        editor_context.static_objects_mutex.unlock();

        object->select();
    }

    editor_context.gizmo->update_visibility();
}

void DeleteObjectsCommand::update_description()
{
    std::snprintf(description, COMMAND_DESCRIPTION_BUFFER_SIZE,
        "Delete objects: to delete: %zu objects",
        objects_.size()
    );
}
