#ifndef EXAFMM_TACO_COMMON_H_
#define EXAFMM_TACO_COMMON_H_

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

#include "taco/taco.h"

struct CellAttr {
  vec3 X;
  real_t R;
  vecP M;
  vecP L;
};

typedef taco::Cell<3, real_t, Body, 0, kvec4, CellAttr> TacoCell;
typedef taco::Region<3, real_t> Region;
typedef taco::ParticleIterator<3, real_t, Body, 0, kvec4, CellAttr> TacoParticleIterator;


#endif // EXAFMM_TACO_COMMON_H_
