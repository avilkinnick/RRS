#include "editor/ObjectSelector.h"

#include "editor/Action.h"
#include "editor/Camera.h"
#include "editor/EditorContext.h"
#include "editor/Gizmo.h"
#include "editor/Keyboard.h"
#include "editor/Mouse.h"
#include "editor/Route.h"
#include "editor/RouteObject.h"
#include "editor/commands/CommandManager.h"
#include "editor/commands/DeleteObjectsCommand.h"
#include "editor/commands/PasteObjectsCommand.h"
#include "editor/commands/RotateObjectsCommand.h"
#include "editor/commands/ScaleObjectsCommand.h"
#include "editor/commands/SelectObjectsCommand.h"
#include "editor/commands/TranslateObjectsCommand.h"
#include "editor/editor_math.h"

#include <vsg/core/Mask.h>
#include <vsg/maths/common.h>
#include <vsg/maths/transform.h>
#include <vsg/maths/vec3.h>
#include <vsg/nodes/Node.h>
#include <vsg/ui/KeyEvent.h>
#include <vsg/ui/PointerEvent.h>
#include <vsg/utils/LineSegmentIntersector.h>

ObjectSelector::ObjectSelector(EditorContext& context)
    : editor_context(context)
{
}

void ObjectSelector::apply(vsg::ButtonReleaseEvent& buttonRelease)
{
    editor_context.gizmo->apply(buttonRelease);
}

void ObjectSelector::apply(vsg::MoveEvent& moveEvent)
{
    editor_context.gizmo->apply(moveEvent);
}
