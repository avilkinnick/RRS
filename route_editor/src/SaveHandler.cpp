#include "SaveHandler.h"

#include "Action.h"
#include "EditorContext.h"
#include "Journal.h"
#include "Keyboard.h"
#include "RouteObject.h"
#include "filesystem.h"

#include <vsg/core/ref_ptr.h>
#include <vsg/maths/vec3.h>

#include <filesystem>
#include <fstream>
#include <mutex>
#include <string>

SaveHandler::SaveHandler(
    EditorContext& editor_context,
    const std::string& route_dir
)
    : editor_context(editor_context)
    , route_dir_{route_dir}
{
}

void SaveHandler::apply([[maybe_unused]] vsg::KeyPressEvent& keyPress)
{
    if (editor_context.keyboard->pressed_once(ACTION_SAVE_ROUTE))
    {
        save_route();
    }
}

void SaveHandler::save_route() const
{
    const FileSystem& fs{FileSystem::getInstance()};
    const std::string save_dir{fs.combinePath(route_dir_, "topology", "map")};

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

    std::lock_guard<std::mutex> lock_guard{editor_context.static_objects_mutex};
    for (const auto& object : editor_context.static_objects)
    {
        const vsg::dvec3& translation{object->get_translation()};
        const vsg::dvec3 rotation_deg{-object->get_rotation_deg()};

        route_map_file << object->label << "," <<
            translation.x << "," << translation.y << "," << translation.z << "," <<
            rotation_deg.x << "," << rotation_deg.y << "," << rotation_deg.z << ";\n";
    }
}
