#include <cstdio>
#include <cstdlib>
#include <cmath>

#include "tapas.h"

#define DIM (3)
typedef double real_t;
typedef tapas::Vec<DIM, real_t> Vec;
typedef tapas::Region<DIM, real_t> Region;

struct particle {
  real_t X[4];
};

typedef tapas::BodyInfo<particle, 0> BodyInfo;
typedef tapas::Tapas<DIM, real_t, BodyInfo,
                     particle, tapas::NONE, tapas::HOT> Tapas;

#if 0
static void direct(const particle &p1, particle &a1,
                   const particle &p2, particle &a2) {
  const float eps2 = 1e-6;
  real_t dx = p1.X[0] - p2.X[0];
  real_t dy = p1.X[1] - p2.X[1];
  real_t dz = p1.X[2] - p2.X[2];
  real_t R2 = dx * dx + dy * dy + dz * dz + eps2;
  real_t invR = 1.0 / std::sqrt(R2);
  real_t invR3 = invR * invR * invR;
  a1.X[0] += dx * invR3;
  a1.X[1] += dy * invR3;
  a1.X[2] += dz * invR3;
  a1.X[3] += p2.X[3] * invR;
  a2.X[0] += -dx * invR3;
  a2.X[1] += -dy * invR3;
  a2.X[2] += -dz * invR3;
  a2.X[3] += p1.X[3] * invR;
}
// only c1 and c2 can be modified
static void interact(Cell &c1, Cell &c2) {
  if (c1.IsLeaf() && c2.IsLeaf()) {
    tapas::Map(direct, tapas::Product(c1.bodies(), c2.bodies()));
  } else if (c1.IsLeaf()) {
    tapas::Map(interact, tapas::Product(c1, c2.subcells()));
  } else if (c2.IsLeaf()) {
    tapas::Map(interact, tapas::Product(c1.subcells(), c2));
  } else {
    tapas::Map(interact, tapas::Product(c1.subcells(), c2.subcells()));
  }
}

#else
static void direct(Tapas::BodyIterator &p1, Tapas::BodyIterator &p2) {
  const float eps2 = 1e-6;
  real_t dx = p1->X[0] - p2->X[0];
  real_t dy = p1->X[1] - p2->X[1];
  real_t dz = p1->X[2] - p2->X[2];
  real_t R2 = dx * dx + dy * dy + dz * dz + eps2;
  real_t invR = 1.0 / std::sqrt(R2);
  real_t invR3 = invR * invR * invR;
  p1.attr().X[0] += dx * invR3;
  p1.attr().X[1] += dy * invR3;
  p1.attr().X[2] += dz * invR3;
  p1.attr().X[3] += p2->X[3] * invR;
  p2.attr().X[0] += -dx * invR3;
  p2.attr().X[1] += -dy * invR3;
  p2.attr().X[2] += -dz * invR3;
  p2.attr().X[3] += p1->X[3] * invR;
}
// only c1 and c2 can be modified
static void interact(Tapas::Cell &c1, Tapas::Cell &c2) {
  if (c1.IsLeaf() && c2.IsLeaf()) {
    tapas::Map(direct, tapas::Product(c1.bodies(), c2.bodies()));
  } else if (c1.IsLeaf()) {
    tapas::Map(interact, tapas::Product(c1, c2.subcells()));
  } else if (c2.IsLeaf()) {
    tapas::Map(interact, tapas::Product(c1.subcells(), c2));
  } else {
    tapas::Map(interact, tapas::Product(c1.subcells(), c2.subcells()));
  }
}

#endif

particle *calc_direct(struct particle *p, size_t np, int s) {
  // PartitionBSP is a function that partitions the given set of
  // particles by the binary space partitioning. The result is a
  // octree for 3D particles and a quadtree for 2D particles.
  Region r(Vec(0.0, 0.0, 0.0), Vec(1.0, 1.0, 1.0));
  Tapas::Cell *root = Tapas::Partition(p, np, r, s);
  tapas::Map(interact, tapas::Product(*root, *root));
  return root->body_attrs();
}
