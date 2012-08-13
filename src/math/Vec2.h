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

struct Vec2;
inline Vec2 vec2(float x, float y);

struct Vec2 {
  float x;
  float y;

  float Real() const { return x; }
  float Imag() const { return y; }
  float Norm() const { return x*x + y*y; }
  float Manhattan() const { return fabs(x)+fabs(y); }
  float Magnitude() const { return sqrtf(Norm()); }
  Vec2 Conjugate() const { return vec2(x,-y); }
  float Radians() const { return atan2f(y,x); }
  Vec2 Reflection() const { return vec2(y,x); }
  Vec2 Anticlockwise() const { return vec2(-y, x); }
  Vec2 Clockwise() const { return vec2(y, -x); }
  Vec2 Normalized() const { return (*this) / Magnitude(); }
  
  Vec2 operator+(const Vec2& q) const { return vec2(x+q.x, y+q.y); }
  Vec2 operator-(const Vec2& q) const { return vec2(x-q.x, y-q.y); }
  Vec2 operator-() const { return vec2(-x, -y); }
  Vec2 operator*(const Vec2& q) const { return vec2(x*q.x-y*q.y, x*q.y+y*q.x); }
  Vec2 operator*(float k) const { return vec2(k*x, k*y); }
  Vec2 operator/(const Vec2& q) const {
    float normInv = 1.0f/q.Norm();
    return vec2((x*q.x+y*q.y)*normInv, (q.x*y-x*q.y)*normInv);
  }
  Vec2 operator/(float k) const { return vec2(x/k, y/k); }
  
  Vec2 operator +=(const Vec2& u) { x+=u.x; y+=u.y; return *this; }
  Vec2 operator -=(const Vec2& u) { x-=u.x; y-=u.y; return *this; } 
  Vec2 operator *=(const Vec2& u) { *this = (*this)*u; return *this; }
  Vec2 operator *=(float k) { x*=k; y*=k; return *this; }
  Vec2 operator /=(const Vec2& u) { *this = (*this)/u; return *this; }
  Vec2 operator /=(float k) { x/=k; y/=k; return *this; }
};

inline Vec2 vec2(float x, float y) {
  Vec2 result = { x, y };
  return result;
}

inline Vec2 Polar(float radius, float radians) { 
  return vec2(radius*cosf(radians), radius*sinf(radians)); 
}

inline Vec2 operator*(float k, const Vec2& q) { return vec2(k*q.x, k*q.y); }
inline float Dot(const Vec2& u, const Vec2& v) { return u.x*v.x + u.y*v.y; }
inline float Cross(const Vec2& u, const Vec2& v) { return u.x * v.y - v.x* u.y; }
inline Vec2 Lerp(const Vec2& u, const Vec2& v, float t) { return u + t * (v - u); }

