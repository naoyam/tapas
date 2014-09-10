#include <cstdio>
#include <cstdlib>
#include <cmath>

#include "taco/taco.h"

#define DIM (3)
typedef double real_t;
typedef taco::Vec<DIM, real_t> Vec;
typedef taco::Region<DIM, real_t> Region;


struct particle {
  real_t pos[3];
  real_t m;
};

typedef taco::Cell<DIM, real_t, particle> Cell;

static void direct(const Cell &c1, const Cell &c2) {
  const float eps2 = 1e-6;
  for (int i = 0; i < c1.size(); ++i) {
    particle p1 = *c1.Particle(i);
    real_t xi = p1.pos[0];
    real_t xj = p1.pos[1];
    real_t xk = p1.pos[2];
    for (int j = 0; j < c2.size(); ++j) {
      particle p2 = *c2.Particle(j);
      // do some calc
      real_t dx = p2.pos[0] - xi;
      real_t dy = p2.pos[1] - xj;
      real_t dz = p2.pos[2] - xk;
      real_t R2 = dx * dx + dy * dy + dz * dz + eps2;
      real_t invR = 1.0 / std::sqrt(R2);
      real_t invR3 = p2.m * invR * invR * invR;
      Vec f(dx * invR3, dy * invR3, dz * invR3);
      // By convention, "force" is defined as a vector for each
      // particle. accumulate_force is a special function that
      // accumulates all contributions to the force for a particle.
      taco::AccumulateForce(c1, i, f);
      taco::AccumulateForce(c2, j, -f);
      // TODO: potential?
      real_t p1_pot = p2.m * invR;
      real_t p2_pot = p1.m * invR;
    }
  }
}


// only c1 and c2 can be modified
static void interact(const Cell &c1, const Cell &c2) {
  if (c1.IsLeaf() && c2.IsLeaf()) {
    direct(c1, c2);
  } else if (c1.IsLeaf()) {
    taco::Map(interact, taco::Product(c1, taco::SubCells(c2)));
  } else if (c2.IsLeaf()) {
    taco::Map(interact, taco::Product(taco::SubCells(c1), c2));
  } else {
    taco::Map(interact, taco::Product(taco::SubCells(c1), taco::SubCells(c2)));
  }
}


void calc_direct(struct particle *p, size_t np, int s) {
  // PartitionBSP is a function that partitions the given set of
  // particles by the binary space partitioning. The result is a
  // octree for 3D particles and a quadtree for 2D particles.
  Region r(Vec(0.0, 0.0, 0.0), Vec(1.0, 1.0, 1.0));
  Cell *root = taco::PartitionBSP<DIM, real_t, particle, 0>(p, np, r, s);
  taco::Map(interact, taco::Product(*root, *root));
  
  //TACO_vec *force = get_force(root);
  //return force;
}
