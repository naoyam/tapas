#include <cstdio>
#include <cstdlib>
#include <cmath>

#include "taco/taco.h"

#define DIM (3)
typedef double real_t;
typedef taco::Vec<DIM, real_t> Vec;
typedef taco::Region<DIM, real_t> Region;


struct particle {
  real_t X[4];
};

typedef taco::Cell<DIM, real_t, particle, particle> Cell;

// TODO: make the parameters non constnat
static void direct(Cell &c1, Cell &c2) {
  const float eps2 = 1e-6;
  for (int i = 0; i < c1.size(); ++i) {
    const particle &p1 = c1.particle(i);
    real_t xi = p1.X[0];
    real_t xj = p1.X[1];
    real_t xk = p1.X[2];
    particle fi = {{0, 0, 0, 0}};
    for (int j = 0; j < c2.size(); ++j) {
      const particle &p2 = c2.particle(j);
      real_t dx = p2.X[0] - xi;
      real_t dy = p2.X[1] - xj;
      real_t dz = p2.X[2] - xk;
      real_t R2 = dx * dx + dy * dy + dz * dz + eps2;
      real_t invR = 1.0 / std::sqrt(R2);
      real_t invR3 = p2.X[3] * invR * invR * invR;
      fi.X[0] += dx * invR3;
      fi.X[1] += dy * invR3;
      fi.X[2] += dz * invR3;
      fi.X[3] += p2.X[3] * invR;
      // mutual interaction
      particle &fj = c2.attr(j);
      fj.X[0] += -dx * invR3;
      fj.X[1] += -dy * invR3;
      fj.X[2] += -dz * invR3;
      fj.X[3] += p1.X[3] * invR;
    }
    c1.attr(i).X[0] += fi.X[0];
    c1.attr(i).X[1] += fi.X[1];
    c1.attr(i).X[2] += fi.X[2];
    c1.attr(i).X[3] += fi.X[3];
  }
}


// only c1 and c2 can be modified
static void interact(Cell &c1, Cell &c2) {
  if (c1.IsLeaf() && c2.IsLeaf()) {
    direct(c1, c2);
  } else if (c1.IsLeaf()) {
    taco::Map(interact, taco::Product(c1, c2.subcells()));
  } else if (c2.IsLeaf()) {
    taco::Map(interact, taco::Product(c1.subcells(), c2));
  } else {
    taco::Map(interact, taco::Product(c1.subcells(), c2.subcells()));
  }
}


particle *calc_direct(struct particle *p, size_t np, int s) {
  // PartitionBSP is a function that partitions the given set of
  // particles by the binary space partitioning. The result is a
  // octree for 3D particles and a quadtree for 2D particles.
  Region r(Vec(0.0, 0.0, 0.0), Vec(1.0, 1.0, 1.0));
  Cell *root = taco::PartitionBSP<DIM, real_t, particle, 0, particle>(p, np, r, s);
  particle *out = taco::Map(interact, taco::Product(*root, *root));
  return out;
}
