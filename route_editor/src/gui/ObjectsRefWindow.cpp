#include "editor/gui/ObjectsRefWindow.h"

#include "editor/EditorContext.h"

#include <vsgImGui/imgui.h>

ObjectsRefWindow::ObjectsRefWindow(EditorContext& editor_context,
    ImGuiWindowFlags window_flags)
    : gui_settings(editor_context.gui_settings)
{
}

void ObjectsRefWindow::show() const
{
}
