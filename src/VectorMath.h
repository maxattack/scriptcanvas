#pragma once
#include "Math.h"

// ALIGNED 4D VECTOR MATH

#if NO_SIMD
  #define USE_SSE3      0
  #define USE_NEON      0
  #define VEC4_ALIGNED
#elif defined(__ARM_NEON__)
  // For iPhones
  #define USE_SSE3      0
  #define USE_NEON      1
  #define VEC4_ALIGNED   __attribute__((aligned(16)))
  #include <arm_neon.h>
#elif defined(__SSE3__)
  // For x86 Laptops/Desktops
  #define USE_SSE3      1
  #define USE_NEON      0
  #define VEC4_ALIGNED   __attribute__((aligned(16)))
  #include <immintrin.h>
#else
  // Mirr... scalar portability fallback :P
  #define USE_SSE3      0
  #define USE_NEON      0
  #define VEC4_ALIGNED  
#endif

union vec4 {
  struct {
    float x, y, z, w;
  };
  float values[4];
  #if USE_NEON
    float32x4_t neon;
  #elif USE_SSE3
    inline __m128 sse() const { return _mm_load_ps(values); }
  #endif

  float Magnitude() const {
    #if USE_NEON
        float32x4_t v1 = vmulq_f32(neon, neon);
        float32x2_t v2 = vpadd_f32(vget_low_f32(v1), vget_high_f32(v1));
        v2 = vpadd_f32(v2, v2);
        return sqrt(vget_lane_f32(v2, 0));
    #elif USE_SSE3
      const __m128 q = sse();
      const __m128 product = q * q;
      const __m128 halfsum = _mm_hadd_ps(product, product);
      return _mm_cvtss_f32(_mm_sqrt_ss(_mm_hadd_ps(halfsum, halfsum)));
    #else
        return sqrtf(x*x + y*y + z*z + w*w);
    #endif    
  }

  vec4 operator-() const {
    #if USE_NEON
        float32x4_t v = vnegq_f32(neon);
        return *(vec4 *)&v;
    #elif USE_SSE3
        __m128 v = _mm_set1_ps(0) - sse();
        return *(vec4 *)&v;
    #else
        vec4 v = {{ -x, -y, -z, -w }};
        return v;
    #endif
  }

  vec4 operator+(const vec4& q) const {
    #if USE_NEON
        float32x4_t v = vaddq_f32(neon, q.neon);
        return *(vec4 *)&v;
    #elif USE_SSE3
        __m128 v = sse() + q.sse();
        return *(vec4 *)&v;
    #else
        vec4 v = {{ x + q.x, y + q.y, z + q.z, w + q.w }};
        return v;
    #endif
  }

  vec4 operator-(const vec4& q) const {
    #if USE_NEON
        float32x4_t v = vsubq_f32(neon, q.neon);
        return *(vec4 *)&v;
    #elif USE_SSE3
        __m128 v = sse() - q.sse();
        return *(vec4 *)&v;
    #else
        vec4 v = {{ x - q.x, y - q.y, z - q.z, w - q.w }};
        return v;
    #endif
  }

  vec4 operator*(const vec4& q) const {
    #if USE_NEON
        float32x4_t v = vmulq_f32(neon, q.neon);
        return *(vec4 *)&v;
    #elif USE_SSE3
        __m128 v = sse() * q.sse();
        return *(vec4 *)&v;
    #else
        vec4 v = {{ x * q.x, y * q.y, z * q.z, w * q.w }};
        return v;
    #endif
  }

  vec4 operator/(const vec4& q) const {
    #if USE_NEON
      float32x4_t *vLeft = &neon;
      float32x4_t *vRight = &q.neon;
      float32x4_t estimate = vrecpeq_f32(*vRight);    
      estimate = vmulq_f32(vrecpsq_f32(*vRight, estimate), estimate);
      estimate = vmulq_f32(vrecpsq_f32(*vRight, estimate), estimate);
      float32x4_t v = vmulq_f32(*vLeft, estimate);
      return *(vec4 *)&v;
    #elif USE_SSE3
      __m128 v = sse() / q.sse();
      return *(vec4 *)&v;
    #else
        vec4 v = {{ x / q.x, y / q.y, z / q.z, w / q.w }};
        return v;
    #endif
  }

  vec4 operator*(float k) const {
    #if USE_NEON
        float32x4_t v = vmulq_f32(neon, vdupq_n_f32(k));
        return *(vec4 *)&v;
    #elif USE_SSE3
        __m128 v = sse() * _mm_set1_ps(k);
        return *(vec4 *)&v;
    #else
        vec4 v = {{ x * k, y * k, z * k, w * k }};
        return v;   
    #endif
  }

  vec4 operator/(float k) const {
    #if USE_NEON
        float32x4_t values = vdupq_n_f32((float32_t)k);
        float32x4_t estimate = vrecpeq_f32(values);    
        estimate = vmulq_f32(vrecpsq_f32(values, estimate), estimate);
        estimate = vmulq_f32(vrecpsq_f32(values, estimate), estimate);
        float32x4_t v = vmulq_f32(neon, estimate);
        return *(vec4 *)&v;
    #elif USE_SSE3
        __m128 v = sse() / _mm_set1_ps(k);
        return *(vec4 *)&v;
    #else
        vec4 v = {{ x / k, y / k, z / k, w / k }};
        return v;
    #endif    
  }
  
  vec4 operator +=(const vec4& u) { *this = (*this) + u; return *this; };
  vec4 operator -=(const vec4& u) { *this = (*this) - u; return *this; };
  vec4 operator *=(const vec4& u) { *this = (*this) * u; return *this; };
  vec4 operator /=(const vec4& u) { *this = (*this) / u; return *this; };

  vec4 operator *=(float k) { *this = (*this) * k; return *this; };
  vec4 operator /=(float k) { *this = (*this) / k; return *this; };

} VEC4_ALIGNED;

inline vec4 Vec4(float x=0.f, float y=0.f, float z=0.f, float w=0.0f) {
  vec4 result = {{ x, y, z, w }};
  return result;
}

inline vec4 Vec4(float values[4]) {
  vec4 result = {{ values[0], values[1], values[2], values[3] }};
  return result;
}

inline vec4 Vec4(float2_t f2, float z=0.f, float w=0.f) {
  vec4 result = {{ f2.x, f2.y, z, w }};
  return result;
}

inline vec4 operator*(float k, const vec4& q) { return q * k; }

inline float Dot(vec4 u, vec4 v) {
  #if USE_NEON
    float32x4_t v1 = vmulq_f32(u.neon, v.neon);
    float32x2_t v2 = vpadd_f32(vget_low_f32(v1), vget_high_f32(v1));
    v2 = vpadd_f32(v2, v2);
    return vget_lane_f32(v2, 0);
  #elif USE_SSE3
    const __m128 product = u.sse() * v.sse();
    const __m128 halfsum = _mm_hadd_ps(product, product);
    return _mm_cvtss_f32(_mm_hadd_ps(halfsum, halfsum));
  #else
    return u.x * v.x + u.y * v.y + u.z * v.z + u.w * v.w;
  #endif
}

inline vec4 Cross(vec4 u, vec4 v) {
  #if USE_SSE3
    const __m128 vl = u.sse();
    const __m128 vr = v.sse();
      __m128 vLeft1  = _mm_shuffle_ps(vl, vl, _MM_SHUFFLE(3, 0, 2, 1));
      __m128 vRight1 = _mm_shuffle_ps(vr, vr, _MM_SHUFFLE(3, 1, 0, 2));
      __m128 vLeft2  = _mm_shuffle_ps(vl, vl, _MM_SHUFFLE(3, 1, 0, 2));
      __m128 vRight2 = _mm_shuffle_ps(vr, vr, _MM_SHUFFLE(3, 0, 2, 1));
      vLeft1 = vLeft1 * vRight1;
      vLeft2 = vLeft2 * vRight2;
      vLeft1 = vLeft1 - vLeft2;
      //Set last element to 0
      uint32_t mask[4] = {0xffffffff, 0xffffffff, 0xffffffff, 0x0};
      vLeft1 = _mm_and_ps(vLeft1, _mm_load_ps((float *)mask));
      return *(vec4 *)&vLeft1;
  #else
      vec4 result = {{  
        u.y * v.z - u.z * v.y,
        u.z * v.x - u.x * v.z,
        u.x * v.y - u.y * v.x,
        0.0f 
      }};
      return result;
  #endif  
}

inline vec4 Lerp(vec4 u, vec4 v, float t) {
  #if USE_NEON
    float32x4_t vDiff = vsubq_f32(v.neon, u.neon);
    vDiff = vmulq_f32(vDiff, vdupq_n_f32((float32_t)t));
    float32x4_t result = vaddq_f32(*(float32x4_t *)&u, vDiff);
    return *(vec4 *)&result;
  #elif USE_SSE3
    const __m128 s = u.sse();
    const __m128 result = s + (v.sse() * _mm_set1_ps(t));
    return *(vec4 *)&result;
  #else
    vec4 result = {{ 
      u.x + ((v.x - u.x) * t),
      u.y + ((v.y - u.y) * t),
      u.z + ((v.z - u.z) * t),
      u.w + ((v.w - u.w) * t) 
    }};
    return result;
  #endif  
}

union mat4 {
  struct {
      float m00, m01, m02, m03;
      float m10, m11, m12, m13;
      float m20, m21, m22, m23;
      float m30, m31, m32, m33;
  };
  struct {
    vec4 u, v, w, t;
  };
  vec4 cols[4];
  float m[16];

  vec4 Column(int col) const {
    #if USE_NEON
        float32x4_t v = vld1q_f32(&(m[col * 4]));
        return *(vec4 *)&v;
    #elif USE_SSE3
        __m128 v = cols[col].sse();
        return *(vec4 *)&v;
    #else
        return cols[col];
    #endif
  }

  void SetColumn(int col, vec4 vector) {
    #if USE_NEON
        float *dst = &(m[col * 4]);
        vst1q_f32(dst, vld1q_f32(vector.v));
    #elif USE_SSE3
        *((__m128*)&m[col*4]) = *(__m128*)&vector;
    #else
        m[col * 4 + 0] = vector.values[0];
        m[col * 4 + 1] = vector.values[1];
        m[col * 4 + 2] = vector.values[2];
        m[col * 4 + 3] = vector.values[3];
    #endif
  }

  mat4 Transpose() const {
    #if USE_NEON
        float32x4x4_t m = vld4q_f32(m);
        return *(mat4 *)&m;
    #else
        mat4 result = {{ 
          m[0], m[4], m[8], m[12],
          m[1], m[5], m[9], m[13],
          m[2], m[6], m[10], m[14],
          m[3], m[7], m[11], m[15] 
        }};
        return result;
    #endif
  }

  mat4 operator*(const mat4& r) const {
    #if USE_NEON
        float32x4x4_t iMatrixLeft = *(float32x4x4_t *)this;
        float32x4x4_t iMatrixRight = *(float32x4x4_t *)&r;
        float32x4x4_t result;

        result.val[0] = vmulq_n_f32(iMatrixLeft.val[0], vgetq_lane_f32(iMatrixRight.val[0], 0));
        result.val[1] = vmulq_n_f32(iMatrixLeft.val[0], vgetq_lane_f32(iMatrixRight.val[1], 0));
        result.val[2] = vmulq_n_f32(iMatrixLeft.val[0], vgetq_lane_f32(iMatrixRight.val[2], 0));
        result.val[3] = vmulq_n_f32(iMatrixLeft.val[0], vgetq_lane_f32(iMatrixRight.val[3], 0));

        result.val[0] = vmlaq_n_f32(result.val[0], iMatrixLeft.val[1], vgetq_lane_f32(iMatrixRight.val[0], 1));
        result.val[1] = vmlaq_n_f32(result.val[1], iMatrixLeft.val[1], vgetq_lane_f32(iMatrixRight.val[1], 1));
        result.val[2] = vmlaq_n_f32(result.val[2], iMatrixLeft.val[1], vgetq_lane_f32(iMatrixRight.val[2], 1));
        result.val[3] = vmlaq_n_f32(result.val[3], iMatrixLeft.val[1], vgetq_lane_f32(iMatrixRight.val[3], 1));

        result.val[0] = vmlaq_n_f32(result.val[0], iMatrixLeft.val[2], vgetq_lane_f32(iMatrixRight.val[0], 2));
        result.val[1] = vmlaq_n_f32(result.val[1], iMatrixLeft.val[2], vgetq_lane_f32(iMatrixRight.val[1], 2));
        result.val[2] = vmlaq_n_f32(result.val[2], iMatrixLeft.val[2], vgetq_lane_f32(iMatrixRight.val[2], 2));
        result.val[3] = vmlaq_n_f32(result.val[3], iMatrixLeft.val[2], vgetq_lane_f32(iMatrixRight.val[3], 2));

        result.val[0] = vmlaq_n_f32(result.val[0], iMatrixLeft.val[3], vgetq_lane_f32(iMatrixRight.val[0], 3));
        result.val[1] = vmlaq_n_f32(result.val[1], iMatrixLeft.val[3], vgetq_lane_f32(iMatrixRight.val[1], 3));
        result.val[2] = vmlaq_n_f32(result.val[2], iMatrixLeft.val[3], vgetq_lane_f32(iMatrixRight.val[2], 3));
        result.val[3] = vmlaq_n_f32(result.val[3], iMatrixLeft.val[3], vgetq_lane_f32(iMatrixRight.val[3], 3));

        return *(mat4 *)&result;
    #elif USE_SSE3

      const __m128 l0 = _mm_load_ps(&m[0]);
      const __m128 l1 = _mm_load_ps(&m[4]);
      const __m128 l2 = _mm_load_ps(&m[8]);
      const __m128 l3 = _mm_load_ps(&m[12]);

      const __m128 r0 = _mm_load_ps(&r.m[0]);
      const __m128 r1 = _mm_load_ps(&r.m[4]);
      const __m128 r2 = _mm_load_ps(&r.m[8]);
      const __m128 r3 = _mm_load_ps(&r.m[12]);
      
      const __m128 m0 = l0 * _mm_shuffle_ps(r0, r0, _MM_SHUFFLE(0, 0, 0, 0))
              + l1 * _mm_shuffle_ps(r0, r0, _MM_SHUFFLE(1, 1, 1, 1))
              + l2 * _mm_shuffle_ps(r0, r0, _MM_SHUFFLE(2, 2, 2, 2))
              + l3 * _mm_shuffle_ps(r0, r0, _MM_SHUFFLE(3, 3, 3, 3));

      const __m128 m1 = l0 * _mm_shuffle_ps(r1, r1, _MM_SHUFFLE(0, 0, 0, 0))
              + l1 * _mm_shuffle_ps(r1, r1, _MM_SHUFFLE(1, 1, 1, 1))
              + l2 * _mm_shuffle_ps(r1, r1, _MM_SHUFFLE(2, 2, 2, 2))
              + l3 * _mm_shuffle_ps(r1, r1, _MM_SHUFFLE(3, 3, 3, 3));

      const __m128 m2 = l0 * _mm_shuffle_ps(r2, r2, _MM_SHUFFLE(0, 0, 0, 0))
              + l1 * _mm_shuffle_ps(r2, r2, _MM_SHUFFLE(1, 1, 1, 1))
              + l2 * _mm_shuffle_ps(r2, r2, _MM_SHUFFLE(2, 2, 2, 2))
              + l3 * _mm_shuffle_ps(r2, r2, _MM_SHUFFLE(3, 3, 3, 3));

      const __m128 m3 = l0 * _mm_shuffle_ps(r3, r3, _MM_SHUFFLE(0, 0, 0, 0))
              + l1 * _mm_shuffle_ps(r3, r3, _MM_SHUFFLE(1, 1, 1, 1))
              + l2 * _mm_shuffle_ps(r3, r3, _MM_SHUFFLE(2, 2, 2, 2))
              + l3 * _mm_shuffle_ps(r3, r3, _MM_SHUFFLE(3, 3, 3, 3));
            
      mat4 result;
      _mm_store_ps(&result.m[0], m0);
      _mm_store_ps(&result.m[4], m1);
      _mm_store_ps(&result.m[8], m2);
      _mm_store_ps(&result.m[12], m3);
      return result;

    #else
        mat4 result;
        
        result.m[0]  = m[0] * r.m[0]  + m[4] * r.m[1]  + m[8] * r.m[2]   + m[12] * r.m[3];
        result.m[4]  = m[0] * r.m[4]  + m[4] * r.m[5]  + m[8] * r.m[6]   + m[12] * r.m[7];
        result.m[8]  = m[0] * r.m[8]  + m[4] * r.m[9]  + m[8] * r.m[10]  + m[12] * r.m[11];
        result.m[12] = m[0] * r.m[12] + m[4] * r.m[13] + m[8] * r.m[14]  + m[12] * r.m[15];
          
        result.m[1]  = m[1] * r.m[0]  + m[5] * r.m[1]  + m[9] * r.m[2]   + m[13] * r.m[3];
        result.m[5]  = m[1] * r.m[4]  + m[5] * r.m[5]  + m[9] * r.m[6]   + m[13] * r.m[7];
        result.m[9]  = m[1] * r.m[8]  + m[5] * r.m[9]  + m[9] * r.m[10]  + m[13] * r.m[11];
        result.m[13] = m[1] * r.m[12] + m[5] * r.m[13] + m[9] * r.m[14]  + m[13] * r.m[15];
          
        result.m[2]  = m[2] * r.m[0]  + m[6] * r.m[1]  + m[10] * r.m[2]  + m[14] * r.m[3];
        result.m[6]  = m[2] * r.m[4]  + m[6] * r.m[5]  + m[10] * r.m[6]  + m[14] * r.m[7];
        result.m[10] = m[2] * r.m[8]  + m[6] * r.m[9]  + m[10] * r.m[10] + m[14] * r.m[11];
        result.m[14] = m[2] * r.m[12] + m[6] * r.m[13] + m[10] * r.m[14] + m[14] * r.m[15];
          
        result.m[3]  = m[3] * r.m[0]  + m[7] * r.m[1]  + m[11] * r.m[2]  + m[15] * r.m[3];
        result.m[7]  = m[3] * r.m[4]  + m[7] * r.m[5]  + m[11] * r.m[6]  + m[15] * r.m[7];
        result.m[11] = m[3] * r.m[8]  + m[7] * r.m[9]  + m[11] * r.m[10] + m[15] * r.m[11];
        result.m[15] = m[3] * r.m[12] + m[7] * r.m[13] + m[11] * r.m[14] + m[15] * r.m[15];
        
        return result;
    #endif    
  }

  vec4 operator*(vec4 vectorRight) const {
    #if USE_NEON
        float32x4x4_t iMatrix = *(float32x4x4_t *)this;
        float32x4_t v;
        
        iMatrix.val[0] = vmulq_n_f32(iMatrix.val[0], (float32_t)vectorRight.values[0]);
        iMatrix.val[1] = vmulq_n_f32(iMatrix.val[1], (float32_t)vectorRight.values[1]);
        iMatrix.val[2] = vmulq_n_f32(iMatrix.val[2], (float32_t)vectorRight.values[2]);
        iMatrix.val[3] = vmulq_n_f32(iMatrix.val[3], (float32_t)vectorRight.values[3]);
        
        iMatrix.val[0] = vaddq_f32(iMatrix.val[0], iMatrix.val[1]);
        iMatrix.val[2] = vaddq_f32(iMatrix.val[2], iMatrix.val[3]);
        
        v = vaddq_f32(iMatrix.val[0], iMatrix.val[2]);
        
        return *(vec4 *)&v;
    #elif USE_SSE3
      const __m128 v = _mm_load_ps(&vectorRight.values[0]);
      const __m128 r = _mm_load_ps(&m[0])  * _mm_shuffle_ps(v, v, _MM_SHUFFLE(0, 0, 0, 0))
                     + _mm_load_ps(&m[4])  * _mm_shuffle_ps(v, v, _MM_SHUFFLE(1, 1, 1, 1))
                     + _mm_load_ps(&m[8])  * _mm_shuffle_ps(v, v, _MM_SHUFFLE(2, 2, 2, 2))
                     + _mm_load_ps(&m[12]) * _mm_shuffle_ps(v, v, _MM_SHUFFLE(3, 3, 3, 3));
        
      vec4 ret;
      *(__m128*)&ret = r;
        return ret;
    #else
        vec4 v = {{ 
          m[0] * vectorRight.values[0] + m[4] * vectorRight.values[1] + m[8] * vectorRight.values[2] + m[12] * vectorRight.values[3],
          m[1] * vectorRight.values[0] + m[5] * vectorRight.values[1] + m[9] * vectorRight.values[2] + m[13] * vectorRight.values[3],
          m[2] * vectorRight.values[0] + m[6] * vectorRight.values[1] + m[10] * vectorRight.values[2] + m[14] * vectorRight.values[3],
          m[3] * vectorRight.values[0] + m[7] * vectorRight.values[1] + m[11] * vectorRight.values[2] + m[15] * vectorRight.values[3] 
        }};
        return v;
    #endif    
  }

} VEC4_ALIGNED;

inline mat4 Mat4(
  float m00=1, float m01=0, float m02=0, float m03=0,
  float m10=0, float m11=1, float m12=0, float m13=0,
  float m20=0, float m21=0, float m22=1, float m23=0,
  float m30=0, float m31=0, float m32=0, float m33=1
) {
  mat4 result = {{
    m00, m01, m02, m03,
    m10, m11, m12, m13,
    m20, m21, m22, m23,
    m30, m31, m32, m33 
  }};
  return result;
}

inline mat4 Mat4(transform_t t, float z=0.f) {
  mat4 result = {{
    t.q.x, t.q.y, 0.f, 0.f,
    -t.q.y, t.q.x, 0.f, 0.f,
    0.f, 0.f, 1.f, 0.f,
    t.t.x, t.t.y, z, 1.f
  }};
  return result;
}

inline mat4 Mat4(ztransform_t t) {
  mat4 result = {{
    t.t.q.x, t.t.q.y, 0.f, 0.f,
    -t.t.q.y, t.t.q.x, 0.f, 0.f,
    0.f, 0.f, 1.f, 0.f,
    t.t.t.x, t.t.t.y, t.z, 1.f
  }};
  return result;
}

inline mat4 Mat4(float values[16]) {
  mat4 m = {{ 
    values[0], values[1], values[2], values[3],
    values[4], values[5], values[6], values[7],
    values[8], values[9], values[10], values[11],
    values[12], values[13], values[14], values[15] 
  }};
  return m;
}

inline mat4 MatTranspose4(
  float m00, float m01, float m02, float m03,
  float m10, float m11, float m12, float m13,
  float m20, float m21, float m22, float m23,
  float m30, float m31, float m32, float m33
) {
  mat4 result = {{
    m00, m10, m20, m30,
    m01, m11, m21, m31,
    m02, m12, m22, m32,
    m03, m13, m23, m33
  }};
  return result;
}

inline mat4 MatTranspose4(float values[16]) {
  #if USE_NEON
    float32x4x4_t m = vld4q_f32(values);
    return *(mat4 *)&m;
  #else
    mat4 m = {{
      values[0], values[4], values[8], values[12],
      values[1], values[5], values[9], values[13],
      values[2], values[6], values[10], values[14],
      values[3], values[7], values[11], values[15] 
    }};
    return m;
  #endif
}

inline mat4 Mat4(vec4 col0, vec4 col1, vec4 col2, vec4 col3) {
  #if USE_NEON
    float32x4x4_t m;
    result.val[0] = vld1q_f32(col0.v);
    result.val[1] = vld1q_f32(col1.v);
    result.val[2] = vld1q_f32(col2.v);
    result.val[3] = vld1q_f32(col3.v);
    return *(mat4 *)&m;
  #elif USE_SSE3
    mat4 result;
    *((__m128*)&result.m[0])  = *(__m128*)&col0;
    *((__m128*)&result.m[4])  = *(__m128*)&col1;
    *((__m128*)&result.m[8])  = *(__m128*)&col2;
    *((__m128*)&result.m[12]) = *(__m128*)&col3;
    return result;
  #else
    mat4 result = {{ 
      col0.x, col0.y, col0.z, col0.w,
      col1.x, col1.y, col1.z, col1.w,
      col2.x, col2.y, col2.z, col2.w,
      col3.x, col3.y, col3.z, col3.w 
    }};
    return result;
  #endif  
}

// VECTORIZED PARAMETRIC CURVES
// These compute curves based on linear multiplication by a "cubic parameterc vector":
// U = < u^3, u^2, u, 1 >,

#define ORTH_ROTATION_MAT  (Mat4(0, 1, 0, 0, -1, 0, 0, 0))
#define IDENTITY_MAT  (Mat4())

inline mat4 HermiteMatrix(vec4 p0, vec4 p1, vec4 t0, vec4 t1) {
	return Mat4(p0, p1, t0, t1) * Mat4(
    2, -2, 1, 1, 
    -3, 3, -2, -1, 
    0, 0, 1, 0, 
    1, 0, 0, 0
  );
}

inline mat4 HermiteDerivMatrix(vec4 p0, vec4 p1, vec4 t0, vec4 t1) {
  return Mat4(p0, p1, t0, t1) * Mat4(
    0, 0, 0, 0, 
    6, -6, 3, 3, 
    -6, 6, -4, -2, 
    0, 0, 1, 0
  );
}

inline mat4 HermiteNormMatrix(vec4 p0, vec4 p1, vec4 t0, vec4 t1) {
  return ORTH_ROTATION_MAT * HermiteDerivMatrix(p0, p1, t0, t1);
}

inline mat4 BezierMatrix(vec4 p0, vec4 p1, vec4 p2, vec4 p3) {
	return Mat4(p0, p1, p2, p3) * Mat4(
    -1, 3, -3, 1, 
    3, -6, 3, 0, 
    -3, 3, 0, 0, 
    1, 0, 0, 0
  );
}

inline mat4 BezierDerivMatrix(vec4 p0, vec4 p1, vec4 p2, vec4 p3) {
  return Mat4(p0, p1, p2, p3) * Mat4(
    0, 0, 0, 0, 
    -3, 9, -9, 3, 
    6, -12, 6, 0, 
    -3, 3, 0, 0
  );
}

inline mat4 BezierNormMatrix(vec4 p0, vec4 p1, vec4 t0, vec4 t1) {
  return ORTH_ROTATION_MAT * BezierDerivMatrix(p0, p1, t0, t1);
}

inline mat4 QuadraticBezierMatrix(vec4 p0, vec4 p1, vec4 p2) {
  return Mat4(Vec4(), p0, p1, p2) * Mat4(
    0, 0, 0, 0,
    0, 1, -2, 1,
    0, -2, 2, 0,
    0, 1, 0, 0
  );
}

inline mat4 QuadraticBezierDerivMatrix(vec4 p0, vec4 p1, vec4 p2) {
  return Mat4(Vec4(), p0, p1, p2) * Mat4(
    0, 0, 0, 0,
    0, 0, 0, 0,
    0, 2, -4, 2,
    0, -2, 2, 0
  );
}

inline mat4 QuadraticBezierNormMatrix(vec4 p0, vec4 p1, vec4 p2) {
  return ORTH_ROTATION_MAT * QuadraticBezierDerivMatrix(p0, p1, p2);
}

