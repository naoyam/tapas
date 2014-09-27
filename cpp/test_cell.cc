#include "tapas/stdcbug.h"

#include <iostream>
#include <cmath>
#include <cassert>

#include "tapas.h"

#include "tapas/logging.h"

#define TEST_DIM (3)
#include "test_util.h"

//using namespace std;

typedef tapas::BodyInfo<particle, 0> BodyInfo;
typedef tapas::Tapas<TEST_DIM, real_t, BodyInfo,
                     particle, int, tapas::HOT> Tapas;

#ifdef N
const int N = N;
#else
const int N = 1 << 10;
#endif

#ifndef S
#define S (10)
#endif

static void count(Tapas::Cell &c) {
  if (c.IsLeaf()) {
    c.attr() = c.nb();
    std::cerr << "Leaf: key: " << c.key() << ", count: "
              << c.attr() << std::endl;
  } else {
    tapas::Map(count, c.subcells());
    for (int i = 0; i < c.nsubcells(); ++i) {
      Tapas::Cell &sc = c.subcell(i);
      c.attr() += sc.attr();
    }
    std::cerr << "key: " << c.key() << ", count: "
              << c.attr() << std::endl;
  }
}

int main(int argc, char *argv[]) {
  Region r(Vec(0.0, 0.0, 0.0), Vec(1.0, 1.0, 1.0));
  int np = N;
  //const int depth_bit_width = tapas::CalcMinBitLen(max_depth);
  particle *p = GetParticles(np, r);

  Tapas::Cell *root = Tapas::Partition(p, np, r, S);
  tapas::Map(count, *root);
  
  return 0;
}

