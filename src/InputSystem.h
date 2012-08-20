#pragma once
#include "math/Vec.h"
struct Input {
	Vec mousePosition;
	double seconds;
};

// These methods are all synchronized
void InitializeInputSystem();
void SetMousePosition(int x, int y);
void SetTime(double t);
void GetInput(Input* outInput);
void FinalizeInputSystem();
