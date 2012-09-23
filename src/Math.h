#pragma once
#include "Foundation.h"
#include <cmath>
#include <stdint.h>

// CONSTANTS

#define kTau            (M_PI+M_PI)
#define kColinearSlop   (0.0001f)

// UTILITY FUNCTIONS

inline float Clamp(float u, float lo=0.f, float hi=1.f) {
  return u<lo?lo:u>hi?hi:u;
}

inline float Clamp01(uint8_t byte) {
  return (byte*255.f) / 255.f;
}

// 2D VECTOR MATH

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

// 2D PARAMETRIC FUNCTIONS

bool LinearIntersection(float2 u0, float2 u1, float2 v0, float2 v1, float& u);
bool LinearIntersection(float2 u0, float2 u1, float2 v0, float2 v1, float& u, float& v);

float2 QuadraticBezier(float2 p0, float2 p1, float2 p2, float u);
float2 QuadraticBezierDeriv(float2 p0, float2 p1, float2 p2, float u);
float2 CubicBezier(float2 p0, float2 p1, float2 p2, float2 p3, float u);
float2 CubicBezierDeriv(float2 p0, float2 p1, float2 p2, float2 p3, float u);
float2 CubicHermite(float2 p0, float2 m0, float2 p1, float2 m1, float u);
float2 CubicHermiteDeriv(float2 p0, float2 m0, float2 p1, float2 m1, float u);

// 2D COMPLEX TRANSFORMS OF THE FORM: q*v + t

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
