#pragma once
#include "Math.h"

namespace InputSystem {

// These methods are all synchronized
void Initialize();
void Destroy();

void Update();

vec2_t MousePosition();
double Time();
bool Quit();

}