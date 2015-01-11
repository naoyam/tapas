#ifndef EXAFMM_TAPAS_COMMON_H_
#define EXAFMM_TAPAS_COMMON_H_

#include "types.h" // exafmm/include/types.h

#include "tapas.h"
#include "tapas/single_node_morton_hot.h" // Morton-key based single node partitioning

struct CellAttr {
    real_t R;
    vecP M;
    vecP L;
};

typedef tapas::BodyInfo<Body, 0> BodyInfo;
typedef tapas::Tapas<3, real_t, BodyInfo, kvec4, CellAttr, tapas::SingleNodeMortonHOT> Tapas;
typedef Tapas::Region Region;

namespace tapas_kernel {

void P2M(Tapas::Cell &C);
void M2M(Tapas::Cell &C);
void M2L(Tapas::Cell &Ci, Tapas::Cell &Cj, vec3 Xperiodic, bool mutual);
void L2L(Tapas::Cell &C);
void L2P(Tapas::BodyIterator &B);
void P2P(Tapas::BodyIterator &Ci, Tapas::BodyIterator &Cj, vec3 Xperiodic);

} // tapas_kernel

#endif // EXAFMM_TAPAS_COMMON_H_
