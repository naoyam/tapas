#ifndef taco_kernel_h_
#define taco_kernel_h_

#include "taco_common.h"

namespace taco_kernel {

#if 0
void P2M(TacoCell &C);
void M2M(TacoCell &C);
void M2L(TacoCell &Ci, TacoCell &Cj, vec3 Xperiodic, bool mutual);
void L2L(TacoCell &C);
void L2P(TacoCell &C);
void P2P(TacoCell &Ci, TacoCell &Cj, vec3 Xperiodic, bool mutual);
void P2P(TacoCell &C);
#else
void P2M(TacoCell &C);
void M2M(TacoCell &C);
void M2L(TacoCell &Ci, TacoCell &Cj, vec3 Xperiodic, bool mutual);
void L2L(TacoCell &C);
void L2P(TacoParticleIterator &B);
void P2P(TacoParticleIterator &Ci, TacoParticleIterator &Cj, vec3 Xperiodic);
#endif
} // taco_kernel

#endif // taco_kernel_h_
