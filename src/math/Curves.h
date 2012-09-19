// ScriptCanvas: An Artful, Interactive Canvas
// Copyright (C) 2011 max.kaufmann@gmail.com
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.

#pragma once

#include "float2.h"

bool LinearIntersection(float2 u0, float2 u1, float2 v0, float2 v1, float& u);
bool LinearIntersection(float2 u0, float2 u1, float2 v0, float2 v1, float& u, float& v);

float2 QuadraticBezier(float2 p0, float2 p1, float2 p2, float u);
float2 QuadraticBezierDeriv(float2 p0, float2 p1, float2 p2, float u);
float2 CubicBezier(float2 p0, float2 p1, float2 p2, float2 p3, float u);
float2 CubicBezierDeriv(float2 p0, float2 p1, float2 p2, float2 p3, float u);
float2 CubicHermite(float2 p0, float2 m0, float2 p1, float2 m1, float u);
float2 CubicHermiteDeriv(float2 p0, float2 m0, float2 p1, float2 m1, float u);
