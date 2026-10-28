#include "editor/StaticObjectManager.h"

#include "editor/EditorContext.h"
#include "editor/Mask.h"
#include "editor/Outline.h"
#include "editor/Route.h"

#include <Journal.h>

#include <vsg/core/Mask.h>
#include <vsg/core/ref_ptr.h>
#include <vsg/maths/common.h>
#include <vsg/maths/transform.h>
#include <vsg/maths/vec3.h>
#include <vsg/nodes/MatrixTransform.h>
#include <vsg/nodes/PagedLOD.h>
#include <vsg/nodes/Switch.h>

#include <QString>

#include <cstddef>
#include <string>
#include <utility>

static constexpr vsg::dvec3 X_AXIS = {1.0, 0.0, 0.0};
static constexpr vsg::dvec3 Y_AXIS = {0.0, 1.0, 0.0};
static constexpr vsg::dvec3 Z_AXIS = {0.0, 0.0, 1.0};

StaticObjectManager::StaticObjectManager(EditorContext& editor_ctx,
                                         std::size_t max_object_count)
    : editor_ctx(editor_ctx)
    , labels(max_object_count)
    , rel_paths(max_object_count)
    , mat_transforms(max_object_count)
    , vsg_switches(max_object_count)
    , paged_lod_switches(max_object_count)
    , outline_switches(max_object_count, nullptr)
    , init_mats(max_object_count)
    , is_selected(max_object_count)
    , is_hidden(max_object_count)
    , generations(max_object_count, 0)
    , object_count(0)
    , max_object_count(max_object_count)
{
}

// TODO: Add multithreading
bool StaticObjectManager::add_object(
    const std::string& label, const std::string& rel_path,
    const vsg::ref_ptr<vsg::PagedLOD>& paged_lod, const vsg::dvec3& pos,
    const vsg::dvec3& rot_deg, const vsg::dvec3& scale, bool is_selected,
    bool is_hidden)
{
    if (object_count >= max_object_count)
    {
        Journal::instance()->error(
            QString("Could not add more static objects.\n"
                    "Max static object's count: %1").arg(max_object_count));

        return false;
    }

    labels[object_count] = label;
    rel_paths[object_count] = rel_path;

    // const auto outline = editor_ctx.outline_builder->create_outline(paged_lod);

    const auto vsg_switch = vsg::Switch::create();
    vsg_switch->addChild(vsg::Mask{MASK_SCENE | MASK_CLICKABLE}, paged_lod);
    // vsg_switch->addChild(vsg::MASK_OFF, outline);

    vsg_switches[object_count] = vsg_switch;
    paged_lod_switches[object_count] = &vsg_switch->children[0];
    // outline_switches[object_count] = &vsg_switch->children[1];

    const auto mat_transform = vsg::MatrixTransform::create();
    mat_transform->addChild(vsg_switch);
    mat_transform->matrix = vsg::translate(pos) *
                            vsg::rotate(vsg::radians(rot_deg.z), Z_AXIS) *
                            vsg::rotate(vsg::radians(rot_deg.y), Y_AXIS) *
                            vsg::rotate(vsg::radians(rot_deg.x), X_AXIS) *
                            vsg::scale(scale);

    mat_transforms[object_count] = mat_transform;

    editor_ctx.compile_info_switch_nodes.lock()->emplace_back(
        CompileInfoSwitchNode{editor_ctx.route, mat_transform, vsg::MASK_ALL});

    this->is_selected[object_count] = is_selected;
    this->is_hidden[object_count] = is_hidden;

    ++generations[object_count];

    ++object_count;

    return true;
}

bool StaticObjectManager::remove_object(std::size_t id)
{
    if (id >= object_count)
    {
        return false;
    }

    const std::size_t last_object_id = object_count - 1;
    if (id == last_object_id)
    {
        --object_count;
        return true;
    }

    labels[id] = std::move(labels[last_object_id]);
    rel_paths[id] = std::move(rel_paths[last_object_id]);

    mat_transforms[id] = mat_transforms[last_object_id];
    vsg_switches[id] = vsg_switches[last_object_id];
    paged_lod_switches[id] = paged_lod_switches[last_object_id];
    outline_switches[id] = outline_switches[last_object_id];

    is_selected[id] = is_selected[last_object_id];
    is_hidden[id] = is_hidden[last_object_id];

    ++generations[id];

    --object_count;

    return true;
}

bool StaticObjectManager::select_object(std::size_t id)
{
    if (id >= object_count)
    {
        return false;
    }

    return true;
}
