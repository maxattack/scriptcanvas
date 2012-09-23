#pragma once
#include "Math.h"

namespace InputSystem {

// These methods are all synchronized
void Initialize();
void SetMousePosition(int x, int y);
void SetTime(double t);

float2 MousePosition();
void GetMousePosition(int *x=0, int *y=0);
double Time();

void Finalize();

}