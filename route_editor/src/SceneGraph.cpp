#include "SceneGraph.h"

#include "EditorContext.h"
#include "Mask.h"
#include "Route.h"

#include <vsg/core/Mask.h>
#include <vsg/lighting/AmbientLight.h>

SceneGraph::SceneGraph(
    EditorContext& context,
    vsg::ref_ptr<Route>& route,
    const std::string& route_dir,
    const vsg::ref_ptr<Gizmo>& gizmo,
    ObjectManager& object_manager
)
    : context_(context)
    , route(route)
    , route_dir(route_dir)
    , gizmo(gizmo)
    , object_manager(object_manager)
{
    ambient_light_ = vsg::AmbientLight::create();
    this->addChild(vsg::Mask{MASK_SCENE}, ambient_light_);
}

void SceneGraph::load_route()
{
    route = Route::create(context_, context_.camera_settings, context_.vsg_options,
        route_dir, gizmo, object_manager);

    context_.compile_infos.emplace_back(CompileInfo{
        vsg::ref_ptr(this), route, vsg::MASK_ALL});
}
