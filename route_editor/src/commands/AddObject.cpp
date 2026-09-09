#include "commands/AddObject.h"

#include "Route.h"
#include "RouteObject.h"
#include "EditorContext.h"
#include "Gizmo.h"
#include "commands/Command.h"

#include <vsg/core/Mask.h>
#include <vsg/core/ref_ptr.h>

#include <algorithm>
#include <cstdio>

AddObject::AddObject(EditorContext& context, vsg::ref_ptr<RouteObject> object)
    : Command(context)
    , object_to_add_(object)
    , objects_to_deselect_(context.selected_objects)
{
    update_description();
}

void AddObject::execute()
{
    for (const auto& object : objects_to_deselect_)
    {
        object->deselect();
    }

    context_.compile_infos.lock()->emplace_back(CompileInfo{
        context_.route, object_to_add_, vsg::MASK_ALL});

    context_.static_objects.lock()->emplace_back(object_to_add_);

    context_.deferred_selection.emplace_back(object_to_add_);
}

void AddObject::undo()
{
    object_to_add_->deselect();

    {
        auto static_objects = context_.static_objects.lock();
        static_objects->erase(std::find(static_objects->begin(),
            static_objects->end(), object_to_add_));
    }

    context_.route->children.erase(
        std::find_if(context_.route->children.begin(), context_.route->children.end(),
            [this](const vsg::Switch::Child& child) {
                return child.node == object_to_add_;
            }
        )
    );

    for (const auto& object : objects_to_deselect_)
    {
        object->select();
    }

    context_.compile_infos.lock()->emplace_back(CompileInfo{nullptr, context_.route});

    context_.gizmo->update_visibility();
}

void AddObject::update_description()
{
    std::snprintf(description_, COMMAND_DESCRIPTION_BUFFER_SIZE,
        "Add object: \"%s\"", object_to_add_->label.c_str()
    );
}
