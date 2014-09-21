#ifndef TAPAS_KERNEL_H_
#define TAPAS_KERNEL_H_

#include "tapas_common.h"

namespace tapas_kernel {

#if 0
void P2M(Tapas::Cell &C);
void M2M(Tapas::Cell &C);
void M2L(Tapas::Cell &Ci, Tapas::Cell &Cj, vec3 Xperiodic, bool mutual);
void L2L(Tapas::Cell &C);
void L2P(Tapas::Cell &C);
void P2P(Tapas::Cell &Ci, Tapas::Cell &Cj, vec3 Xperiodic, bool mutual);
void P2P(Tapas::Cell &C);
#else
void P2M(Tapas::Cell &C);
void M2M(Tapas::Cell &C);
void M2L(Tapas::Cell &Ci, Tapas::Cell &Cj, vec3 Xperiodic, bool mutual);
void L2L(Tapas::Cell &C);
void L2P(Tapas::BodyIterator &B);
void P2P(Tapas::BodyIterator &Ci, Tapas::BodyIterator &Cj, vec3 Xperiodic);
#endif
} // tapas_kernel

#endif // TACO_KERNEL_H_
