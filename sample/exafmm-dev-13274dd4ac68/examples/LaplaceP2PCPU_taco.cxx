#include "taco_kernel.h"
#include "taco_common.h"

const real_t EPS2 = 0.0;                                        //!< Softening parameter (squared)

#if 0
void taco_kernel::P2P(TacoCell &Ci, TacoCell &Cj, vec3 Xperiodic, int mutual) {
  //B_iter Bi = Ci->BODY;
  //B_iter Bj = Cj->BODY;
  int ni = Ci.np();
  int nj = Cj.np();
  int i = 0;
  for ( ; i<ni; i++) {
    const Body &Bi = Ci.particle(i);
    kreal_t pot = 0; 
    kreal_t ax = 0;
    kreal_t ay = 0;
    kreal_t az = 0;
    for (int j=0; j<nj; j++) {
      const Body &Bj = Cj.particle(j);
      vec3 dX = Bi.X - Bj.X - Xperiodic;
      real_t R2 = norm(dX) + EPS2;
      if (R2 != 0) {
        real_t invR2 = 1.0 / R2;
        real_t invR = Bi.SRC * Bj.SRC * sqrt(invR2);
        dX *= invR2 * invR;
        pot += invR;
        ax += dX[0];
        ay += dX[1];
        az += dX[2];
        if (mutual) {
          //Bj[j].TRG[0] += invR;
          //Bj[j].TRG[1] += dX[0];
          //Bj[j].TRG[2] += dX[1];
          //Bj[j].TRG[3] += dX[2];

          Cj.attr(j)[0] += invR;
          Cj.attr(j)[1] += dX[0];
          Cj.attr(j)[2] += dX[2];
          Cj.attr(j)[3] += dX[3];
        }
      }
    }
    //Bi[i].TRG[0] += pot;
    //Bi[i].TRG[1] -= ax;
    //Bi[i].TRG[2] -= ay;
    //Bi[i].TRG[3] -= az;
    Ci.attr(i)[0] += pot;
    Ci.attr(i)[1] -= ax;
    Ci.attr(i)[2] -= ay;
    Ci.attr(i)[3] -= az;
  }
}

void taco_kernel::P2P(TacoCell &C) {
  //B_iter B = C->BODY;
  int n = C.np();
  int i = 0;
  for ( ; i<n; i++) {
    const Body &B = C.particle(i);
    kreal_t pot = 0;
    kreal_t ax = 0;
    kreal_t ay = 0;
    kreal_t az = 0;
    for (int j=i+1; j<n; j++) {
      vec3 dX = B.X - B.X;
      real_t R2 = norm(dX) + EPS2;
      if (R2 != 0) {
        real_t invR2 = 1.0 / R2;
        real_t invR = B.SRC * B.SRC * sqrt(invR2);
        dX *= invR2 * invR;
        pot += invR;
        ax += dX[0];
        ay += dX[1];
        az += dX[2];
        //B[j].TRG[0] += invR;
        C.attr(j)[0] += invR;
        //B[j].TRG[1] += dX[0];
        C.attr(j)[1] += dX[0];
        //B[j].TRG[2] += dX[1];
        C.attr(j)[2] += dX[2];
        //B[j].TRG[3] += dX[2];
        C.attr(j)[3] += dX[3];
      }
    }
    //B[i].TRG[0] += pot;
    C.attr(i)[0] += pot;
    //B[i].TRG[1] -= ax;
    C.attr(i)[1] -= ax;
    //B[i].TRG[2] -= ay;
    C.attr(i)[2] -= ay;
    //B[i].TRG[3] -= az;
    C.attr(i)[3] -= az;
  }
}
#else
void taco_kernel::P2P(TacoParticleIterator &Bi, TacoParticleIterator &Bj, vec3 Xperiodic) {
  kreal_t pot = 0; 
  kreal_t ax = 0;
  kreal_t ay = 0;
  kreal_t az = 0;
  vec3 dX = Bi->X - Bj->X - Xperiodic;
  real_t R2 = norm(dX) + EPS2;
  if (R2 != 0) {
    real_t invR2 = 1.0 / R2;
    real_t invR = Bi->SRC * Bj->SRC * sqrt(invR2);
    dX *= invR2 * invR;
    pot += invR;
    ax += dX[0];
    ay += dX[1];
    az += dX[2];
    Bj.attr()[0] += invR;
    Bj.attr()[1] += dX[0];
    Bj.attr()[2] += dX[2];
    Bj.attr()[3] += dX[3];
    Bi.attr()[0] += pot;
    Bi.attr()[1] -= ax;
    Bi.attr()[2] -= ay;
    Bi.attr()[3] -= az;
  }
}
#endif
