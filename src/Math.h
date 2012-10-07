// ScriptCanvas: An Artful, Interactive Canvas
// Copyright (C) 2012 max.kaufmann@gmail.com
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
#include "Foundation.h"
#include <cmath>
#include <stdint.h>

// CONSTANTS

#define kTau            (M_PI+M_PI)
#define kColinearSlop   (0.0001f)

// UTILITY FUNCTIONS

inline float Clamp(float u, float lo=0.f, float hi=1.f) { return u<lo?lo:u>hi?hi:u; }
inline float Clamp01(uint8_t byte) { return (byte*255.f) / 255.f; }
inline float Lerp(float u, float v, float t) { return u + t * (v - u); }

// 2D VECTOR MATH

struct vec2_t;
inline vec2_t Vec2(float x, float y);

struct vec2_t {
  float x;
  float y;

  float Real() const { return x; }
  float Imag() const { return y; }
  float Norm() const { return x*x + y*y; }
  float Manhattan() const { return fabs(x)+fabs(y); }
  float Magnitude() const { return sqrtf(Norm()); }
  vec2_t Conjugate() const { return Vec2(x,-y); }
  float Radians() const { return atan2f(y,x); }
  vec2_t Reflection() const { return Vec2(y,x); }
  vec2_t Anticlockwise() const { return Vec2(-y, x); }
  vec2_t Clockwise() const { return Vec2(y, -x); }
  vec2_t Normalized() const { return (*this) / Magnitude(); }
  
  vec2_t operator+(vec2_t q) const { return Vec2(x+q.x, y+q.y); }
  vec2_t operator-(vec2_t q) const { return Vec2(x-q.x, y-q.y); }
  vec2_t operator-() const { return Vec2(-x, -y); }
  vec2_t operator*(vec2_t q) const { return Vec2(x*q.x-y*q.y, x*q.y+y*q.x); } // cmul
  vec2_t operator*(float k) const { return Vec2(k*x, k*y); }
  vec2_t operator/(vec2_t q) const {
    float normInv = 1.0f/q.Norm();
    return Vec2((x*q.x+y*q.y)*normInv, (q.x*y-x*q.y)*normInv);
  }
  vec2_t operator/(float k) const { return Vec2(x/k, y/k); }
  
  vec2_t operator +=(vec2_t u) { x+=u.x; y+=u.y; return *this; }
  vec2_t operator -=(vec2_t u) { x-=u.x; y-=u.y; return *this; } 
  vec2_t operator *=(vec2_t u) { *this = (*this)*u; return *this; }
  vec2_t operator *=(float k) { x*=k; y*=k; return *this; }
  vec2_t operator /=(vec2_t u) { *this = (*this)/u; return *this; }
  vec2_t operator /=(float k) { x/=k; y/=k; return *this; }
};

inline vec2_t Vec2(float x, float y) {
  vec2_t result = { x, y };
  return result;
}

inline vec2_t Polar(float radius, float radians) { 
  return Vec2(radius*cosf(radians), radius*sinf(radians)); 
}

inline vec2_t operator*(float k, vec2_t q) { return Vec2(k*q.x, k*q.y); }
inline float Dot(vec2_t u, vec2_t v) { return u.x*v.x + u.y*v.y; }
inline float Cross(vec2_t u, vec2_t v) { return u.x * v.y - v.x* u.y; }
inline vec2_t Lerp(vec2_t u, vec2_t v, float t) { return u + t * (v - u); }

// 2D PARAMETRIC FUNCTIONS

bool LinearIntersection(vec2_t u0, vec2_t u1, vec2_t v0, vec2_t v1, float& u);
bool LinearIntersection(vec2_t u0, vec2_t u1, vec2_t v0, vec2_t v1, float& u, float& v);

vec2_t QuadraticBezier(vec2_t p0, vec2_t p1, vec2_t p2, float u);
vec2_t QuadraticBezierDeriv(vec2_t p0, vec2_t p1, vec2_t p2, float u);
vec2_t CubicBezier(vec2_t p0, vec2_t p1, vec2_t p2, vec2_t p3, float u);
vec2_t CubicBezierDeriv(vec2_t p0, vec2_t p1, vec2_t p2, vec2_t p3, float u);
vec2_t CubicHermite(vec2_t p0, vec2_t m0, vec2_t p1, vec2_t m1, float u);
vec2_t CubicHermiteDeriv(vec2_t p0, vec2_t m0, vec2_t p1, vec2_t m1, float u);

// 2D COMPLEX TRANSFORMS OF THE FORM: q*v + t

struct transform_t;
inline transform_t Transform(vec2_t q=Vec2(1,0), vec2_t t=Vec2(0,0));

struct transform_t {

  vec2_t attitude;
  vec2_t translation;

  vec2_t TransformPoint(vec2_t p) const { return attitude*p + translation; }
  vec2_t TransformVector(vec2_t v) const { return attitude*v; }
  vec2_t InvTransformPoint(vec2_t p) const { return (p - translation)/attitude; }
  vec2_t InvTransformVector(vec2_t v) const { return v/attitude; }

  bool Identity() const { 
    return translation.Norm() < 0.001f && (attitude-Vec2(1,0)).Norm() < 0.001f; 
  }
  
  transform_t Inverse() const { 
    vec2_t qInv = Vec2(1,0)/attitude;
    return Transform(qInv, -translation*qInv);
  }

  transform_t operator*(transform_t u) const { 
    return Transform(attitude*u.attitude, u.attitude*translation + u.translation);
  }
};

inline transform_t Transform(vec2_t q, vec2_t t) {
  transform_t result = { q, t };
  return result;
}

inline transform_t Translation(vec2_t t) {
  return Transform(Vec2(1,0), t);
}

inline transform_t Rotation(float radians) {
  return Transform(Polar(1,radians), Vec2(0,0));
}

inline transform_t Scale(float k) {
  return Transform(Vec2(k,0), Vec2(0,0));
}

inline transform_t TRS(vec2_t t, float radians, float scale) {
  return Transform(Polar(scale, radians), t);
}

struct ztransform_t {
  transform_t t;
  float depth;
  ztransform_t operator*(const ztransform_t& rhs) const {
    ztransform_t result = {
      t * rhs.t,
      depth + rhs.depth
    };
    return result;
  }
};

inline ztransform_t ZTransform(vec2_t q=Vec2(1,0), vec2_t t=Vec2(0,0), float depth=0) {
  ztransform_t result = { Transform(q, t), depth };
  return result;
}
