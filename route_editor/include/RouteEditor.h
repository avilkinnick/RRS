#ifndef ROUTE_EDITOR_H
#define ROUTE_EDITOR_H

#include "EditorContext.h"
#include "EditorState.h"
#include "ObjectManager.h"
#include "commands/CommandManager.h"

#include <vsg/io/Options.h>

#include <memory>

class Gizmo;
class Route;
class SceneGraph;
class WindowHandler;

class RouteEditor
{
public:
    RouteEditor();

    ~RouteEditor();

    bool initialize();

    void run();

private:
    void initialize_journal(const char* filename = "editor.log") const;

    void read_settings();

    void create_vsg_options();

    void configure_shaders();

    void compile_models();

    void handle_deferred_selection();

private:
    EditorContext editor_context;
    vsg::ref_ptr<vsg::Viewer> viewer_;
    vsg::ref_ptr<WindowHandler> window_handler_;

    EditorState editor_state = EditorState::SELECT_ROUTE;
    CommandManager command_manager;
    vsg::ref_ptr<SceneGraph> scene_graph;
    vsg::ref_ptr<Route> route;
    std::string route_dir;
    vsg::ref_ptr<Gizmo> gizmo;
    std::unique_ptr<ObjectManager> object_manager;
};

#endif // ROUTE_EDITOR_H
