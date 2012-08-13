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

#include "Vec2.h"

struct Transform {

  Vec2 q;
  Vec2 t;

  Vec2 TransformPoint(Vec2 p) const { return q*p + t; }
  Vec2 TransformVector(Vec2 v) const { return q*v; }
  Vec2 InvTransformPoint(Vec2 p) const { return (p - t)/q; }
  Vec2 InvTransformVector(Vec2 v) const { return v/q; }

  Transform() {}
  Transform(Vec2 aq, Vec2 at) : q(aq), t(at) {}

  inline static Transform Identity() { return Transform(vec2(1,0), vec2(0,0)); }

  bool IsIdentity() const { return t.Norm() < 0.001f && (q-vec2(1,0)).Norm() < 0.001f; }
  
  Transform Inverse() const { 
    Vec2 qInv = vec2(1,0)/q;
    return Transform(qInv, -t*qInv);
  }

  Transform operator*(const Transform& u) const { 
    return Transform(q*u.q, u.q*t + u.t);
  }
};

inline Transform Translation(Vec2 t) {
  return Transform(vec2(1,0), t);
}

inline Transform Rotation(float radians) {
  return Transform(Polar(1,radians), vec2(0,0));
}

inline Transform Scale(float k) {
  return Transform(vec2(k,0), vec2(0,0));
}

inline Transform TRS(Vec2 t, float radians, float scale) {
  return Transform(Polar(scale, radians), t);
}
