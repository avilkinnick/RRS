#ifndef WINDOW_HANDLER_H
#define WINDOW_HANDLER_H

#include <vsg/core/Inherit.h>
#include <vsg/core/Visitor.h>
#include <vsg/core/ref_ptr.h>

class Camera;
struct EditorContext;

namespace vsg
{

class Camera;
class ConfigureWindowEvent;
class Perspective;

}

class WindowHandler : public vsg::Inherit<vsg::Visitor, WindowHandler>
{
public:
    WindowHandler(EditorContext& editor_context);

    virtual void apply(vsg::ConfigureWindowEvent& configureWindow) override;

    void set_camera(const vsg::ref_ptr<Camera>& camera) { this->camera = camera; }

private:
    vsg::ref_ptr<Camera> camera;
};

#endif // WINDOW_HANDLER_H
