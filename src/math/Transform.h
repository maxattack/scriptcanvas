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

struct transform;
inline transform Transform(float2 q=Float2(1,0), float2 t=Float2(0,0));

struct transform {

  float2 q;
  float2 t;

  float2 TransformPoint(float2 p) const { return q*p + t; }
  float2 TransformVector(float2 v) const { return q*v; }
  float2 InvTransformPoint(float2 p) const { return (p - t)/q; }
  float2 InvTransformVector(float2 v) const { return v/q; }

  bool Identity() const { 
    return t.Norm() < 0.001f && (q-Float2(1,0)).Norm() < 0.001f; 
  }
  
  transform Inverse() const { 
    float2 qInv = Float2(1,0)/q;
    return Transform(qInv, -t*qInv);
  }

  transform operator*(transform u) const { 
    return Transform(q*u.q, u.q*t + u.t);
  }
};

inline transform Transform(float2 q, float2 t) {
  transform result = { q, t };
  return result;
}

inline transform Translation(float2 t) {
  return Transform(Float2(1,0), t);
}

inline transform Rotation(float radians) {
  return Transform(Polar(1,radians), Float2(0,0));
}

inline transform Scale(float k) {
  return Transform(Float2(k,0), Float2(0,0));
}

inline transform TRS(float2 t, float radians, float scale) {
  return Transform(Polar(scale, radians), t);
}
