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

struct Vec;
inline Vec vec(float x, float y);

struct Vec {
  float x;
  float y;

  float Real() const { return x; }
  float Imag() const { return y; }
  float Norm() const { return x*x + y*y; }
  float Manhattan() const { return fabs(x)+fabs(y); }
  float Magnitude() const { return sqrtf(Norm()); }
  Vec Conjugate() const { return vec(x,-y); }
  float Radians() const { return atan2f(y,x); }
  Vec Reflection() const { return vec(y,x); }
  Vec Anticlockwise() const { return vec(-y, x); }
  Vec Clockwise() const { return vec(y, -x); }
  Vec Normalized() const { return (*this) / Magnitude(); }
  
  Vec operator+(const Vec& q) const { return vec(x+q.x, y+q.y); }
  Vec operator-(const Vec& q) const { return vec(x-q.x, y-q.y); }
  Vec operator-() const { return vec(-x, -y); }
  Vec operator*(const Vec& q) const { return vec(x*q.x-y*q.y, x*q.y+y*q.x); }
  Vec operator*(float k) const { return vec(k*x, k*y); }
  Vec operator/(const Vec& q) const {
    float normInv = 1.0f/q.Norm();
    return vec((x*q.x+y*q.y)*normInv, (q.x*y-x*q.y)*normInv);
  }
  Vec operator/(float k) const { return vec(x/k, y/k); }
  
  Vec operator +=(const Vec& u) { x+=u.x; y+=u.y; return *this; }
  Vec operator -=(const Vec& u) { x-=u.x; y-=u.y; return *this; } 
  Vec operator *=(const Vec& u) { *this = (*this)*u; return *this; }
  Vec operator *=(float k) { x*=k; y*=k; return *this; }
  Vec operator /=(const Vec& u) { *this = (*this)/u; return *this; }
  Vec operator /=(float k) { x/=k; y/=k; return *this; }
};

inline Vec vec(float x, float y) {
  Vec result = { x, y };
  return result;
}

inline Vec Polar(float radius, float radians) { 
  return vec(radius*cosf(radians), radius*sinf(radians)); 
}

inline Vec operator*(float k, const Vec& q) { return vec(k*q.x, k*q.y); }
inline float Dot(const Vec& u, const Vec& v) { return u.x*v.x + u.y*v.y; }
inline float Cross(const Vec& u, const Vec& v) { return u.x * v.y - v.x* u.y; }
inline Vec Lerp(const Vec& u, const Vec& v, float t) { return u + t * (v - u); }

