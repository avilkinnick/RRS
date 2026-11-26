#include "editor/gui/ObjectsRefWindow.h"

#include "editor/Camera.h"
#include "editor/EditorContext.h"
#include "editor/Route.h"
#include "editor/RouteObject.h"
#include "editor/commands/AddObjectCommand.h"
#include "editor/commands/CommandManager.h"
#include "editor/settings/GuiSettings.h"

#include <vsg/app/ViewMatrix.h>
#include <vsg/maths/vec3.h>
#include <vsgImGui/imgui.h>

#include <algorithm>
#include <cctype>
#include <memory>
#include <string>
#include <utility>

#define SEARCH_BUF_SIZE 256

ObjectsRefWindow::ObjectsRefWindow(EditorContext &editor_ctx,
                                   ImGuiWindowFlags window_flags)
    : editor_ctx(editor_ctx)
    , camera(*editor_ctx.camera)
    , command_manager(*editor_ctx.command_manager)
    , objects_ref(editor_ctx.objects_ref)
    , route(*editor_ctx.route)
    , show_objects_ref(editor_ctx.gui_settings.show_objects_ref)
    , window_flags(window_flags)
{
}

void ObjectsRefWindow::show() const
{
    if (!show_objects_ref)
    {
        return;
    }

    ImGui::Begin("objects.ref", &show_objects_ref, window_flags);

    static char search_buf[SEARCH_BUF_SIZE] = "";
    ImGui::InputText("label", search_buf, SEARCH_BUF_SIZE);

    std::string search_lower = search_buf;
    std::transform(search_lower.begin(), search_lower.end(),
                   search_lower.begin(), ::tolower);

    if (ImGui::BeginTable("ObjectsRefTable", 2,
                          ImGuiTableFlags_RowBg | ImGuiTableFlags_Borders |
                          ImGuiTableFlags_SizingFixedFit))
    {
        for (const auto &[label, rel_path] : objects_ref)
        {
            std::string label_lower = label;
            std::transform(label_lower.begin(), label_lower.end(),
                           label_lower.begin(), ::tolower);

            if (search_buf[0] != '\0' &&
                label_lower.find(search_lower) == std::string::npos)
            {
                continue;
            }

            ImGui::TableNextRow();
            ImGui::TableNextColumn();

            if (ImGui::Button(label.c_str()))
            {
                add_object(label);
            }

            ImGui::TableNextColumn();
            ImGui::Text("%s", rel_path.c_str());
        }

        ImGui::EndTable();
    }

    ImGui::End();
}

void ObjectsRefWindow::add_object(const std::string &label) const
{
    const auto &paged_lod = route.get_paged_lods().at(label);
    const vsg::dvec3 position = camera.get_look_at()->eye +
                                camera.get_front() * 20.0;

    const auto object = RouteObject::create(editor_ctx, paged_lod, label,
                                            position);

    auto command = std::make_unique<AddObjectCommand>(editor_ctx, object);
    command->execute();
    command_manager.push(std::move(command));
}
