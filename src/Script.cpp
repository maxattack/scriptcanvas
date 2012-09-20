#include "Script.h"

CircleManager CircleSystem::inst;

void ScriptInitialize() {
	SceneSystem::RegisterComponentManager(COMPONENT_CIRCLE, &CircleSystem::inst);
}

void ScriptPaint() {
    RenderBuffer *vbuf;
    RenderSystem::RetrieveFromRenderSystem(&vbuf);
    SceneSystem::Update(vbuf);
 	vbuf->circleCount = 0;
	CircleSystem::inst.Update(vbuf);
    RenderSystem::SubmitToRenderSystem(vbuf);
}
