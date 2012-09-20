#include "Script.h"

namespace CircleSystem {
	CircleManager inst;
};

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
