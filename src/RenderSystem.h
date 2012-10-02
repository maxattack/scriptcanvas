#pragma once
#include "NodeSystem.h"

namespace RenderSystem {

void Initialize();
void Destroy();

void Render(CommandBuffer *buf);

GLuint LoadShaderProgram(const char* filename);

}