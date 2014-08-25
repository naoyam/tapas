#ifndef TACO_VEC_H_
#define TACO_VEC_H_

#include <math.h>

#include "primitive_types.h"

#define DeclareVecType(elm_type, len, key) typedef struct vec##len##key {elm_type x[len];} vec##len##key;

// float vectors
DeclareVecType(float, 1, f)
DeclareVecType(float, 2, f)
DeclareVecType(float, 3, f)
// double vectors
DeclareVecType(double, 1, d)
DeclareVecType(double, 2, d)
DeclareVecType(double, 3, d)
#undef DeclareVecType
#if defined(DEFAULT_FP_TYPE_FLOAT)
#define vec1r vec1f
#define vec2r vec2f
#define vec3r vec3f
#elif defined(DEFAULT_FP_TYPE_DOUBLE)
#define vec1r vec1d
#define vec2r vec2d
#define vec3r vec3d    
#endif
#if DEFAULT_NUM_DIM == 1
#define vec vec1r
#elif DEFAULT_NUM_DIM == 2
#define vec vec2r
#elif DEFAULT_NUM_DIM == 3
#define vec vec3r
#endif

// Scalar binary arithmetic ops
#define DeclareVec3ScalarArithOp(elm_type, key, op, opname)                    \
  static inline vec3##key vec3##key##_##opname##_s(const vec3##key v, elm_type x) { \
    vec3##key r = {{(v).x[0] op (x), (v).x[1] op (x), (v).x[2] op (x)}}; \
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
#define vec3r_add_s vec3f_add_s
#define vec3r_sub_s vec3f_sub_s
#define vec3r_mul_s vec3f_mul_s
#define vec3r_div_s vec3f_div_s
#elif defined(DEFAULT_FP_TYPE_DOUBLE)
#define vec3r_add_s vec3d_add_s
#define vec3r_sub_s vec3d_sub_s
#define vec3r_mul_s vec3d_mul_s
#define vec3r_div_s vec3d_div_s
#endif
#if DEFAULT_NUM_DIM == 1
#define vec_add_s vec1r_add_s
#define vec_sub_s vec1r_sub_s
#define vec_mul_s vec1r_mul_s
#define vec_div_s vec1r_div_s
#elif DEFAULT_NUM_DIM == 2
#define vec_add_s vec2r_add_s
#define vec_sub_s vec2r_sub_s
#define vec_mul_s vec2r_mul_s
#define vec_div_s vec2r_div_s
#elif DEFAULT_NUM_DIM == 3
#define vec_add_s vec3r_add_s
#define vec_sub_s vec3r_sub_s
#define vec_mul_s vec3r_mul_s
#define vec_div_s vec3r_div_s
#endif

// Vector binary arithmetic ops
#define DeclareVec3ArithOp(elm_type, key, op, opname)                    \
  static inline vec3##key vec3##key##_##opname(const vec3##key x, const vec3##key y) { \
    vec3##key r = {{(x).x[0] op (y).x[0], (x).x[1] op (y).x[1], (x).x[2] op (y).x[2]}}; \
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
#define vec3r_add vec3f_add
#define vec3r_sub vec3f_sub
#define vec3r_mul vec3f_mul
#define vec3r_div vec3f_div
#elif defined(DEFAULT_FP_TYPE_DOUBLE)
#define vec3r_add vec3d_add
#define vec3r_sub vec3d_sub
#define vec3r_mul vec3d_mul
#define vec3r_div vec3d_div
#endif
#if DEFAULT_NUM_DIM == 1
#define vec_add vec1r_add
#define vec_sub vec1r_sub
#define vec_mul vec1r_mul
#define vec_div vec1r_div
#elif DEFAULT_NUM_DIM == 2
#define vec_add vec2r_add
#define vec_sub vec2r_sub
#define vec_mul vec2r_mul
#define vec_div vec2r_div
#elif DEFAULT_NUM_DIM == 3
#define vec_add vec3r_add
#define vec_sub vec3r_sub
#define vec_mul vec3r_mul
#define vec_div vec3r_div
#endif

// Reduction
#define DeclareVec3Reduce(elm_type, key, op, opname)                           \
  static inline elm_type vec3##key##_reduce_##opname(const vec3##key x) { \
    return (x).x[0] op (x).x[1] op (x).x[2];                            \
  }
DeclareVec3Reduce(float, f, +, sum)
DeclareVec3Reduce(double, d, +, sum)
DeclareVec3Reduce(float, f, *, mul)
DeclareVec3Reduce(double, d, *, mul)
#undef DeclareVec3Reduce
#if defined(DEFAULT_FP_TYPE_FLOAT)
#define vec3r_reduce_sum vec3f_reduce_sum
#define vec3r_reduce_mul vec3f_reduce_mul
#elif defined(DEFAULT_FP_TYPE_DOUBLE)
#define vec3r_reduce_sum vec3d_reduce_sum
#define vec3r_reduce_mul vec3d_reduce_mul
#endif


// Norm
#define DeclareVec3L1norm(elm_type, key, absop)                         \
  static inline elm_type vec3##key##_l1norm(const vec3##key x, const vec3##key y) { \
    return absop((x).x[0]-(x).x[0]) + absop((x).x[1]-(x).x[1]) + absop((x).x[2]-(x).x[2]); \
  }
#define DeclareVec3L2norm(elm_type, key, absop, sqrtop)                 \
  static inline elm_type vec3##key##_l2norm(const vec3##key x, const vec3##key y) { \
  vec3##key r = {{absop((x).x[0] - (x).x[0]), absop((x).x[1] - (x).x[1]), absop((x).x[2] - (x).x[2])}}; \
      elm_type n = sqrtop(vec3##key##_reduce_sum(vec3##key##_mul(r, r))); \
  return n;                                                             \
  }
#define DeclareVec3L2norm2(elm_type, key, absop)                        \
  static inline elm_type vec3##key##_l2norm2(const vec3##key x, const vec3##key y) { \
  vec3##key r = {{absop((x).x[0] - (x).x[0]), absop((x).x[1] - (x).x[1]), absop((x).x[2] - (x).x[2])}}; \
  elm_type n = vec3##key##_reduce_sum(vec3##key##_mul(r, r));           \
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
#define vec3r_l1norm vec3f_l1norm
#define vec3r_l2norm vec3f_l2norm
#define vec3r_l2norm2 vec3f_l2norm2
#elif defined(DEFAULT_FP_TYPE_DOUBLE)
#define vec3r_l1norm vec3d_l1norm
#define vec3r_l2norm vec3d_l2norm
#define vec3r_l2norm2 vec3d_l2norm2
#endif

#endif /* TACO_VEC_H_ */
