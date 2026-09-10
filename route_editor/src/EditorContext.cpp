#include "EditorContext.h"

#include "Camera.h"
#include "Gizmo.h"
#include "Keyboard.h"
#include "Mouse.h"
#include "ObjectManager.h"
#include "ObjectSelector.h"
#include "Outline.h"
#include "Route.h"
#include "RouteObject.h"
#include "StateManager.h"
#include "commands/CommandManager.h"
#include "topology.h"

#include <vsg/app/Window.h>
#include <vsg/commands/ClearAttachments.h>
#include <vsg/nodes/PagedLOD.h>

EditorContext::EditorContext() = default;

EditorContext::~EditorContext() = default;
