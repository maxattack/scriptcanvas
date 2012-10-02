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

#include "Math.h"

bool LinearIntersection(vec2_t u0, vec2_t u1, vec2_t v0, vec2_t v1, float& u) {
  float norm = (v1.y - v0.y)*(u1.x-u0.x) - (v1.x-v0.x)*(u1.y-u0.y);
  if (norm > -kColinearSlop && norm < kColinearSlop) {
    // lines are parallel
    return false;
  }
  norm = 1.0f / norm;
  u = ((v1.x-v0.x)*(u0.y-v0.y) - (v1.y-v0.y)*(u0.x-v0.x)) * norm;
  return true;  
}

bool LinearIntersection(vec2_t u0, vec2_t u1, vec2_t v0, vec2_t v1, float& u, float& v) {
  float norm = (v1.y - v0.y)*(u1.x-u0.x) - (v1.x-v0.x)*(u1.y-u0.y);
  if (norm > -kColinearSlop && norm < kColinearSlop) {
    // lines are parallel
    u = 0.0f;
    v = 0.0f;
    return false;
  }
  norm = 1.0f / norm;
  u = ((v1.x-v0.x)*(u0.y-v0.y) - (v1.y-v0.y)*(u0.x-v0.x)) * norm;
  v = ((u1.x-u0.x)*(u0.y-v0.y) - (u1.y-u0.y)*(u0.x-v0.x)) * norm;
  return true;  
}

vec2_t QuadraticBezier(vec2_t p0, vec2_t p1, vec2_t p2, float u) {
  return ((1.0f-u)*(1.0f-u))*p0 + (2.0f*(1.0f-u)*u)*p1 + (u*u)*p2;
}
    
vec2_t QuadraticBezierDeriv(vec2_t p0, vec2_t p1, vec2_t p2, float u) {
  return (2.0f*(1.0f-u))*(p1-p0) + (2.0f*u)*(p2-p1);
}

vec2_t CubicBezier(vec2_t p0, vec2_t p1, vec2_t p2, vec2_t p3, float u) {
    return 
      ((1.0f-u) * (1.0f-u) * (1.0f-u)) * p0 +
      (3.0f * (1.0f-u) * (1.0f-u) * u) * p1 +
      (3.0f * (1.0f-u) * u * u) * p2 +
      (u * u * u) * p3;
  }
  
vec2_t CubicBezierDeriv(vec2_t p0, vec2_t p1, vec2_t p2, vec2_t p3, float u){
  return 3.0f * (
    (-(1.0f-u) * (1.0f-u)) * p0 +
    (1.0f - 4.0f * u + 3.0f * u * u) * p1 +
    (2.0f * u - 3.0f * u * u) * p2 +
    (u * u) * p3
  );
}
vec2_t CubicHermite(vec2_t p0, vec2_t m0, vec2_t p1, vec2_t m1, float u) {
  return (2.f*u*u*u - 3.f*u*u + 1.f) * p0 + 
    (u*u*u - 2.f*u*u + u) * m0 + 
    (-2.f*u*u*u + 3.f *u*u) * p1 + 
    (u*u*u - u*u) * m1;
}

vec2_t CubicHermiteDeriv(vec2_t p0, vec2_t m0, vec2_t p1, vec2_t m1, float u) {
  return (6.f*(u*u - u)) * p0 + 
    (3.f*u*u - 4.f*u + 1.f) * m0 + 
    (6.f*(u - u*u)) * p1 + 
    (3.f*u*u - 2.f*u) * m1;
}

