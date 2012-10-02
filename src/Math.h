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

struct float2_t;
inline float2_t Float2(float x, float y);

struct float2_t {
  float x;
  float y;

  float Real() const { return x; }
  float Imag() const { return y; }
  float Norm() const { return x*x + y*y; }
  float Manhattan() const { return fabs(x)+fabs(y); }
  float Magnitude() const { return sqrtf(Norm()); }
  float2_t Conjugate() const { return Float2(x,-y); }
  float Radians() const { return atan2f(y,x); }
  float2_t Reflection() const { return Float2(y,x); }
  float2_t Anticlockwise() const { return Float2(-y, x); }
  float2_t Clockwise() const { return Float2(y, -x); }
  float2_t Normalized() const { return (*this) / Magnitude(); }
  
  float2_t operator+(float2_t q) const { return Float2(x+q.x, y+q.y); }
  float2_t operator-(float2_t q) const { return Float2(x-q.x, y-q.y); }
  float2_t operator-() const { return Float2(-x, -y); }
  float2_t operator*(float2_t q) const { return Float2(x*q.x-y*q.y, x*q.y+y*q.x); }
  float2_t operator*(float k) const { return Float2(k*x, k*y); }
  float2_t operator/(float2_t q) const {
    float normInv = 1.0f/q.Norm();
    return Float2((x*q.x+y*q.y)*normInv, (q.x*y-x*q.y)*normInv);
  }
  float2_t operator/(float k) const { return Float2(x/k, y/k); }
  
  float2_t operator +=(float2_t u) { x+=u.x; y+=u.y; return *this; }
  float2_t operator -=(float2_t u) { x-=u.x; y-=u.y; return *this; } 
  float2_t operator *=(float2_t u) { *this = (*this)*u; return *this; }
  float2_t operator *=(float k) { x*=k; y*=k; return *this; }
  float2_t operator /=(float2_t u) { *this = (*this)/u; return *this; }
  float2_t operator /=(float k) { x/=k; y/=k; return *this; }
};

inline float2_t Float2(float x, float y) {
  float2_t result = { x, y };
  return result;
}

inline float2_t Polar(float radius, float radians) { 
  return Float2(radius*cosf(radians), radius*sinf(radians)); 
}

inline float2_t operator*(float k, float2_t q) { return Float2(k*q.x, k*q.y); }
inline float Dot(float2_t u, float2_t v) { return u.x*v.x + u.y*v.y; }
inline float Cross(float2_t u, float2_t v) { return u.x * v.y - v.x* u.y; }
inline float2_t Lerp(float2_t u, float2_t v, float t) { return u + t * (v - u); }

// 2D PARAMETRIC FUNCTIONS

bool LinearIntersection(float2_t u0, float2_t u1, float2_t v0, float2_t v1, float& u);
bool LinearIntersection(float2_t u0, float2_t u1, float2_t v0, float2_t v1, float& u, float& v);

float2_t QuadraticBezier(float2_t p0, float2_t p1, float2_t p2, float u);
float2_t QuadraticBezierDeriv(float2_t p0, float2_t p1, float2_t p2, float u);
float2_t CubicBezier(float2_t p0, float2_t p1, float2_t p2, float2_t p3, float u);
float2_t CubicBezierDeriv(float2_t p0, float2_t p1, float2_t p2, float2_t p3, float u);
float2_t CubicHermite(float2_t p0, float2_t m0, float2_t p1, float2_t m1, float u);
float2_t CubicHermiteDeriv(float2_t p0, float2_t m0, float2_t p1, float2_t m1, float u);

// 2D COMPLEX TRANSFORMS OF THE FORM: q*v + t

struct transform_t;
inline transform_t Transform(float2_t q=Float2(1,0), float2_t t=Float2(0,0));

struct transform_t {

  float2_t q;
  float2_t t;

  float2_t TransformPoint(float2_t p) const { return q*p + t; }
  float2_t TransformVector(float2_t v) const { return q*v; }
  float2_t InvTransformPoint(float2_t p) const { return (p - t)/q; }
  float2_t InvTransformVector(float2_t v) const { return v/q; }

  bool Identity() const { 
    return t.Norm() < 0.001f && (q-Float2(1,0)).Norm() < 0.001f; 
  }
  
  transform_t Inverse() const { 
    float2_t qInv = Float2(1,0)/q;
    return Transform(qInv, -t*qInv);
  }

  transform_t operator*(transform_t u) const { 
    return Transform(q*u.q, u.q*t + u.t);
  }
};

inline transform_t Transform(float2_t q, float2_t t) {
  transform_t result = { q, t };
  return result;
}

inline transform_t Translation(float2_t t) {
  return Transform(Float2(1,0), t);
}

inline transform_t Rotation(float radians) {
  return Transform(Polar(1,radians), Float2(0,0));
}

inline transform_t Scale(float k) {
  return Transform(Float2(k,0), Float2(0,0));
}

inline transform_t TRS(float2_t t, float radians, float scale) {
  return Transform(Polar(scale, radians), t);
}

struct ztransform_t {
  float z;
  transform_t t;
  ztransform_t operator*(const ztransform_t& rhs) const {
    ztransform_t result = {
      z + rhs.z,
      t * rhs.t
    };
    return result;
  }
};

inline ztransform_t ZTransform() {
  ztransform_t result = { 0, Transform() };
  return result;
}
