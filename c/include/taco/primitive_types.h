#ifndef TACO_PRIMITIVE_TYPES_H_
#define TACO_PRIMITIVE_TYPES_H_

#include "taco/common.h"

#if defined(DEFAULT_FP_TYPE_FLOAT)
#define TACO_real_t float
#elif defined(DEFAULT_FP_TYPE_DOUBLE)
#define TACO_real_t double
#endif

typedef unsigned TACO_index_t;

#endif /* TACO_PRIMITIVE_TYPES_H_ */
