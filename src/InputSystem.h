#pragma once
#include "Math.h"

namespace InputSystem {

// These methods are all synchronized
void Initialize();
void Update();

float2_t MousePosition();
double Time();

void Destroy();

}