#ifndef SAVE_HANDLER_H
#define SAVE_HANDLER_H

#include <vsg/core/Inherit.h>
#include <vsg/core/Visitor.h>
#include <vsg/core/ref_ptr.h>

struct EditorContext;

namespace vsg
{

class KeyPressEvent;

}

// This is temp class to move out functionality from Keyboard
class SaveHandler : public vsg::Inherit<vsg::Visitor, SaveHandler>
{
public:
    SaveHandler(EditorContext& editor_context);

    virtual void apply(vsg::KeyPressEvent& keyPress) override;

private:
    void save_route() const;

private:
    EditorContext& editor_context;
};

#endif // SAVE_HANDLER_H
