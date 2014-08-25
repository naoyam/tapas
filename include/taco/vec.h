#ifndef TACO_VEC_H_
#define TACO_VEC_H_

#include <math.h>

#include "taco/common.h"
#include "taco/primitive_types.h"

#define DeclareVecType(elm_type, len, key) \
  typedef struct TACO_vec##len##key {elm_type x[len];} TACO_vec##len##key;

// float vectors
DeclareVecType(float, 1, f)
DeclareVecType(float, 2, f)
DeclareVecType(float, 3, f)
DeclareVecType(float, 4, f)
// double vectors
DeclareVecType(double, 1, d)
DeclareVecType(double, 2, d)
DeclareVecType(double, 3, d)
DeclareVecType(double, 4, d)
#undef DeclareVecType
#if defined(DEFAULT_FP_TYPE_FLOAT)
#define TACO_vec1r TACO_vec1f
#define TACO_vec2r TACO_vec2f
#define TACO_vec3r TACO_vec3f
#define TACO_vec4r TACO_vec4f
#elif defined(DEFAULT_FP_TYPE_DOUBLE)
#define TACO_vec1r TACO_vec1d
#define TACO_vec2r TACO_vec2d
#define TACO_vec3r TACO_vec3d
#define TACO_vec4r TACO_vec4d    
#endif
#if DEFAULT_NUM_DIM == 1
#define TACO_vec TACO_vec1r
#elif DEFAULT_NUM_DIM == 2
#define TACO_vec TACO_vec2r
#elif DEFAULT_NUM_DIM == 3
#define TACO_vec TACO_vec3r
#elif DEFAULT_NUM_DIM == 4
#define TACO_vec TACO_vec4r
#endif

// Scalar binary arithmetic ops
#define DeclareVec3ScalarArithOp(elm_type, key, op, opname)                    \
  static inline TACO_vec3##key TACO_vec3##key##_##opname##_s(const TACO_vec3##key v, elm_type x) { \
    TACO_vec3##key r = {{(v).x[0] op (x), (v).x[1] op (x), (v).x[2] op (x)}}; \
        return r;                                                       \
  }                                                                     
DeclareVec3ScalarArithOp(float, f, +, add)
DeclareVec3ScalarArithOp(float, f, -, sub)
DeclareVec3ScalarArithOp(float, f, *, mul)
DeclareVec3ScalarArithOp(float, f, /, div)
DeclareVec3ScalarArithOp(double, d, +, add)
DeclareVec3ScalarArithOp(double, d, -, sub)
DeclareVec3ScalarArithOp(double, d, *, mul)
DeclareVec3ScalarArithOp(double, d, /, div)
#undef DeclareVec3ScalarArithOp
#if defined(DEFAULT_FP_TYPE_FLOAT)
#define TACO_vec3r_add_s TACO_vec3f_add_s
#define TACO_vec3r_sub_s TACO_vec3f_sub_s
#define TACO_vec3r_mul_s TACO_vec3f_mul_s
#define TACO_vec3r_div_s TACO_vec3f_div_s
#elif defined(DEFAULT_FP_TYPE_DOUBLE)
#define TACO_vec3r_add_s TACO_vec3d_add_s
#define TACO_vec3r_sub_s TACO_vec3d_sub_s
#define TACO_vec3r_mul_s TACO_vec3d_mul_s
#define TACO_vec3r_div_s TACO_vec3d_div_s
#endif
#if DEFAULT_NUM_DIM == 1
#define TACO_vec_add_s TACO_vec1r_add_s
#define TACO_vec_sub_s TACO_vec1r_sub_s
#define TACO_vec_mul_s TACO_vec1r_mul_s
#define TACO_vec_div_s TACO_vec1r_div_s
#elif DEFAULT_NUM_DIM == 2
#define TACO_vec_add_s TACO_vec2r_add_s
#define TACO_vec_sub_s TACO_vec2r_sub_s
#define TACO_vec_mul_s TACO_vec2r_mul_s
#define TACO_vec_div_s TACO_vec2r_div_s
#elif DEFAULT_NUM_DIM == 3
#define TACO_vec_add_s TACO_vec3r_add_s
#define TACO_vec_sub_s TACO_vec3r_sub_s
#define TACO_vec_mul_s TACO_vec3r_mul_s
#define TACO_vec_div_s TACO_vec3r_div_s
#endif

// Vector binary arithmetic ops
#define DeclareVec3ArithOp(elm_type, key, op, opname)                    \
  static inline TACO_vec3##key TACO_vec3##key##_##opname(const TACO_vec3##key x, const TACO_vec3##key y) { \
    TACO_vec3##key r = {{(x).x[0] op (y).x[0], (x).x[1] op (y).x[1], (x).x[2] op (y).x[2]}}; \
        return r;                                                       \
  }                                                                     
DeclareVec3ArithOp(float, f, +, add)
DeclareVec3ArithOp(float, f, -, sub)
DeclareVec3ArithOp(float, f, *, mul)
DeclareVec3ArithOp(float, f, /, div)
DeclareVec3ArithOp(double, d, +, add)
DeclareVec3ArithOp(double, d, -, sub)
DeclareVec3ArithOp(double, d, *, mul)
DeclareVec3ArithOp(double, d, /, div)
#undef DeclareVec3ArithOp
#if defined(DEFAULT_FP_TYPE_FLOAT)
#define TACO_vec3r_add TACO_vec3f_add
#define TACO_vec3r_sub TACO_vec3f_sub
#define TACO_vec3r_mul TACO_vec3f_mul
#define TACO_vec3r_div TACO_vec3f_div
#elif defined(DEFAULT_FP_TYPE_DOUBLE)
#define TACO_vec3r_add TACO_vec3d_add
#define TACO_vec3r_sub TACO_vec3d_sub
#define TACO_vec3r_mul TACO_vec3d_mul
#define TACO_vec3r_div TACO_vec3d_div
#endif
#if DEFAULT_NUM_DIM == 1
#define TACO_vec_add TACO_vec1r_add
#define TACO_vec_sub TACO_vec1r_sub
#define TACO_vec_mul TACO_vec1r_mul
#define TACO_vec_div TACO_vec1r_div
#elif DEFAULT_NUM_DIM == 2
#define TACO_vec_add TACO_vec2r_add
#define TACO_vec_sub TACO_vec2r_sub
#define TACO_vec_mul TACO_vec2r_mul
#define TACO_vec_div TACO_vec2r_div
#elif DEFAULT_NUM_DIM == 3
#define TACO_vec_add TACO_vec3r_add
#define TACO_vec_sub TACO_vec3r_sub
#define TACO_vec_mul TACO_vec3r_mul
#define TACO_vec_div TACO_vec3r_div
#endif

// Reduction
#define DeclareVec3Reduce(elm_type, key, op, opname)                           \
  static inline elm_type TACO_vec3##key##_reduce_##opname(const TACO_vec3##key x) { \
    return (x).x[0] op (x).x[1] op (x).x[2];                            \
  }
DeclareVec3Reduce(float, f, +, sum)
DeclareVec3Reduce(double, d, +, sum)
DeclareVec3Reduce(float, f, *, mul)
DeclareVec3Reduce(double, d, *, mul)
#undef DeclareVec3Reduce
#if defined(DEFAULT_FP_TYPE_FLOAT)
#define TACO_vec3r_reduce_sum TACO_vec3f_reduce_sum
#define TACO_vec3r_reduce_mul TACO_vec3f_reduce_mul
#elif defined(DEFAULT_FP_TYPE_DOUBLE)
#define TACO_vec3r_reduce_sum TACO_vec3d_reduce_sum
#define TACO_vec3r_reduce_mul TACO_vec3d_reduce_mul
#endif


// Norm
#define DeclareVec3L1norm(elm_type, key, absop)                         \
  static inline elm_type TACO_vec3##key##_l1norm(const TACO_vec3##key x, const TACO_vec3##key y) { \
    return absop((x).x[0]-(y).x[0]) + absop((x).x[1]-(y).x[1]) + absop((x).x[2]-(y).x[2]); \
  }
#define DeclareVec3L2norm(elm_type, key, absop, sqrtop)                 \
  static inline elm_type TACO_vec3##key##_l2norm(const TACO_vec3##key x, const TACO_vec3##key y) { \
  TACO_vec3##key r = {{absop((x).x[0]-(y).x[0]), absop((x).x[1]-(y).x[1]), absop((x).x[2]-(y).x[2])}}; \
      elm_type n = sqrtop(TACO_vec3##key##_reduce_sum(TACO_vec3##key##_mul(r, r))); \
  return n;                                                             \
  }
#define DeclareVec3L2norm2(elm_type, key, absop)                        \
  static inline elm_type TACO_vec3##key##_l2norm2(const TACO_vec3##key x, const TACO_vec3##key y) { \
  TACO_vec3##key r = {{absop((x).x[0]-(y).x[0]), absop((x).x[1]-(y).x[1]), absop((x).x[2]-(y).x[2])}}; \
  elm_type n = TACO_vec3##key##_reduce_sum(TACO_vec3##key##_mul(r, r));           \
  return n;                                                             \
  }
DeclareVec3L1norm(float, f, fabsf)
DeclareVec3L2norm(float, f, fabsf, sqrtf)
DeclareVec3L2norm2(float, f, fabsf)
DeclareVec3L1norm(double, d, fabs)
DeclareVec3L2norm(double, d, fabs, sqrt)
DeclareVec3L2norm2(double, d, fabs)
#undef DeclareVec3L1Norm
#undef DeclareVec3L2Norm
#undef DeclareVec3L2Norm2
#if defined(DEFAULT_FP_TYPE_FLOAT)
#define TACO_vec3r_l1norm TACO_vec3f_l1norm
#define TACO_vec3r_l2norm TACO_vec3f_l2norm
#define TACO_vec3r_l2norm2 TACO_vec3f_l2norm2
#elif defined(DEFAULT_FP_TYPE_DOUBLE)
#define TACO_vec3r_l1norm TACO_vec3d_l1norm
#define TACO_vec3r_l2norm TACO_vec3d_l2norm
#define TACO_vec3r_l2norm2 TACO_vec3d_l2norm2
#endif

#endif /* TACO_VEC_H_ */
