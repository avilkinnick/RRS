#ifndef OBJECT_SELECTOR_H
#define OBJECT_SELECTOR_H

#include <vsg/core/Inherit.h>
#include <vsg/core/Visitor.h>
#include <vsg/core/ref_ptr.h>
#include <vsg/maths/vec3.h>
#include <vulkan/vulkan_core.h>

class Camera;
class CommandManager;
struct EditorContext;
class Gizmo;
class Route;
class RouteObject;
class SingleSwitch;

namespace vsg
{

class ButtonPressEvent;
class ButtonReleaseEvent;
class KeyPressEvent;
class MoveEvent;

}

class ObjectSelector : public vsg::Inherit<vsg::Visitor, ObjectSelector>
{
public:
    ObjectSelector(EditorContext& editor_context);

    void apply(vsg::ButtonReleaseEvent& buttonRelease) override;
    void apply(vsg::MoveEvent& moveEvent) override;

private:
    EditorContext& editor_context;
};

#endif // OBJECT_SELECTOR_H
