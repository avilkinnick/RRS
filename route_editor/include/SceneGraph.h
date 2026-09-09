#ifndef SCENE_GRAPH_H
#define SCENE_GRAPH_H

#include <vsg/core/Inherit.h>
#include <vsg/core/ref_ptr.h>
#include <vsg/nodes/Switch.h>

struct EditorContext;
class Gizmo;
class ObjectManager;
class Route;

class SceneGraph : public vsg::Inherit<vsg::Switch, SceneGraph>
{
public:
    SceneGraph(
        EditorContext& context,
        const std::string& route_dir,
        ObjectManager& object_manager
    );

    void load_route();

private:
    EditorContext& context_;
};

#endif // SCENE_GRAPH_H
