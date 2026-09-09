#include "SceneGraph.h"

#include "EditorContext.h"
#include "Route.h"

#include <vsg/core/Mask.h>

SceneGraph::SceneGraph(
    EditorContext& context,
    const std::string& route_dir,
    ObjectManager& object_manager
)
    : context_(context)
{
    context.route = Route::create(context_, route_dir, context.gizmo, object_manager);
}

void SceneGraph::load_route()
{
    context_.route->load();

    context_.compile_infos.emplace_back(CompileInfo{
        vsg::ref_ptr(this), context_.route, vsg::MASK_ALL});
}
