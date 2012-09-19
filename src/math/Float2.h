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
#include <cmath>

#define kTau (M_PI+M_PI)

struct float2;
inline float2 Float2(float x, float y);

struct float2 {
  float x;
  float y;

  float Real() const { return x; }
  float Imag() const { return y; }
  float Norm() const { return x*x + y*y; }
  float Manhattan() const { return fabs(x)+fabs(y); }
  float Magnitude() const { return sqrtf(Norm()); }
  float2 Conjugate() const { return Float2(x,-y); }
  float Radians() const { return atan2f(y,x); }
  float2 Reflection() const { return Float2(y,x); }
  float2 Anticlockwise() const { return Float2(-y, x); }
  float2 Clockwise() const { return Float2(y, -x); }
  float2 Normalized() const { return (*this) / Magnitude(); }
  
  float2 operator+(float2 q) const { return Float2(x+q.x, y+q.y); }
  float2 operator-(float2 q) const { return Float2(x-q.x, y-q.y); }
  float2 operator-() const { return Float2(-x, -y); }
  float2 operator*(float2 q) const { return Float2(x*q.x-y*q.y, x*q.y+y*q.x); }
  float2 operator*(float k) const { return Float2(k*x, k*y); }
  float2 operator/(float2 q) const {
    float normInv = 1.0f/q.Norm();
    return Float2((x*q.x+y*q.y)*normInv, (q.x*y-x*q.y)*normInv);
  }
  float2 operator/(float k) const { return Float2(x/k, y/k); }
  
  float2 operator +=(float2 u) { x+=u.x; y+=u.y; return *this; }
  float2 operator -=(float2 u) { x-=u.x; y-=u.y; return *this; } 
  float2 operator *=(float2 u) { *this = (*this)*u; return *this; }
  float2 operator *=(float k) { x*=k; y*=k; return *this; }
  float2 operator /=(float2 u) { *this = (*this)/u; return *this; }
  float2 operator /=(float k) { x/=k; y/=k; return *this; }
};

inline float2 Float2(float x, float y) {
  float2 result = { x, y };
  return result;
}

inline float2 Polar(float radius, float radians) { 
  return Float2(radius*cosf(radians), radius*sinf(radians)); 
}

inline float2 operator*(float k, float2 q) { return Float2(k*q.x, k*q.y); }
inline float Dot(float2 u, float2 v) { return u.x*v.x + u.y*v.y; }
inline float Cross(float2 u, float2 v) { return u.x * v.y - v.x* u.y; }
inline float2 Lerp(float2 u, float2 v, float t) { return u + t * (v - u); }

