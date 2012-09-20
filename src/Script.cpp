#include "Script.h"

CircleManager CircleSystem::inst;

void ScriptInitialize() {
	SceneSystem::RegisterComponentManager(COMPONENT_CIRCLE, &CircleSystem::inst);
}

void ScriptUpdate() {
    RenderBuffer *vbuf;
    RenderSystem::RetrieveFromRenderSystem(&vbuf);
    SceneSystem::Update(vbuf);
    CircleSystem::inst.Update(vbuf);
    RenderSystem::SubmitToRenderSystem(vbuf);
}

void ScriptRender(RenderBuffer* vbuf) {
	CircleSystem::inst.Render(vbuf);
}
