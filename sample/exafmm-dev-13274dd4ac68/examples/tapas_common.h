#ifndef EXAFMM_TAPAS_COMMON_H_
#define EXAFMM_TAPAS_COMMON_H_

#include "types.h" // exafmm/include/types.h

#include "tapas.h"

struct CellAttr {
  real_t R;
  vecP M;
  vecP L;
};

typedef tapas::BodyInfo<Body, 0> BodyInfo;
typedef tapas::Tapas<3, real_t, BodyInfo, kvec4, CellAttr, tapas::SingleNodeMortonHOT> Tapas;
typedef Tapas::Region Region;

#endif // EXAFMM_TAPAS_COMMON_H_
