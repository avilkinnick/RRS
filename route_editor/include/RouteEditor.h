#ifndef ROUTE_EDITOR_H
#define ROUTE_EDITOR_H

#include "EditorContext.h"
#include "EditorState.h"
#include "ObjectManager.h"

#include <vsg/io/Options.h>

#include <memory>

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
    std::string route_dir;
    std::unique_ptr<ObjectManager> object_manager;
};

#endif // ROUTE_EDITOR_H
