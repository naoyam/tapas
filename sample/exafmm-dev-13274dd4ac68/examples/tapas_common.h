#ifndef EXAFMM_TAPAS_COMMON_H_
#define EXAFMM_TAPAS_COMMON_H_

#include "args.h"
#include "bound_box.h"
#ifdef CILK
#include "build_tree3.h"
#else
#include "build_tree.h"
#endif
#include "dataset.h"
#include "logger.h"
#include "traversal.h"
#include "up_down_pass.h"
#include "verify.h"
#if VTK
#include "vtk.h"
#endif

#include "tapas.h"

struct CellAttr {
  vec3 X;
  real_t R;
  vecP M;
  vecP L;
};

typedef tapas::BodyInfo<Body, 0> BodyInfo;
typedef tapas::Tapas<3, real_t, BodyInfo,
                     kvec4, CellAttr, tapas::HOT> Tapas;
typedef tapas::Region<3, real_t> Region;


#endif // EXAFMM_TAPAS_COMMON_H_
