#ifndef TAPAS_TEST_UTIL_H_
#define TAPAS_TEST_UTIL_H_

#include <ostream>
#include <cstdlib>

#include "tapas/vec.h"
#include "tapas/basic_types.h"

using namespace std;

#ifndef TEST_DIM
#define TEST_DIM (3)
#endif

typedef double real_t;
typedef tapas::Region<TEST_DIM, real_t> Region;
typedef tapas::Vec<TEST_DIM, real_t> Vec;

struct particle {
  real_t pos[TEST_DIM];
  real_t m;
};

template <int DIM, class FP>
particle * GetParticles(int np, const tapas::Region<DIM, FP> &r) {
  particle *p = new particle[np];
  srand48(0);
  for (int i = 0; i < np; ++i) {
    for (int j = 0; j < DIM; ++j) {
      p[i].pos[j] = drand48() * (r.max()[j] - r.min()[j]) + r.min()[j];
    }
  }
  return p;
}

template <int DIM>
void PrintParticles(particle *pp, int np, std::ostream &os) {
  for (int i = 0; i < np; ++i) {
    particle &p = pp[i];
    for (int j = 0; j < DIM; ++j) {
      os << p.pos[j] << " ";
    }
    os << std::endl;
  }
}

#endif // TAPAS_TEST_UTIL_H_
