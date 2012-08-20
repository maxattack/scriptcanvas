#pragma once
#include "math/Vec.h"

namespace InputSystem {

// These methods are all synchronized
void Initialize();
void SetMousePosition(int x, int y);
void SetTime(double t);

Vec MousePosition();
void GetMousePosition(int *x=0, int *y=0);
double GetTime();

void Finalize();

}