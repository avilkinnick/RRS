#ifndef EDITOR_GUI_OBJECTS_REF_WINDOW_H
#define EDITOR_GUI_OBJECTS_REF_WINDOW_H

#include <vsgImGui/imgui.h>

#include <string>

class Camera;
class CommandManager;
struct EditorContext;
class ObjectsRef;
class Route;

class ObjectsRefWindow
{
public:
    explicit ObjectsRefWindow(EditorContext &editor_ctx,
                              ImGuiWindowFlags window_flags = 0);
    void show() const;

private:
    EditorContext &editor_ctx;

    const Camera &camera;
    CommandManager &command_manager;
    const ObjectsRef &objects_ref;
    Route &route;
    bool &show_objects_ref;
    const ImGuiWindowFlags window_flags;

private:
    void add_object(const std::string &label) const;
};

#endif // EDITOR_GUI_OBJECTS_REF_WINDOW_H
