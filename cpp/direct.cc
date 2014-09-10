#include <cstdio>
#include <cstdlib>

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
  for (int i = 0; i < c1.size(); ++i) {
    particle p1 = *c1.Particle(i);  
    for (int j = 0; j < c2.size(); ++j) {
      particle p2 = *c2.Particle(j);
      // do some calc
      Vec f;
      // By convention, "force" is defined as a vector for each
      // particle. accumulate_force is a special function that
      // accumulates all contributions to the force for a particle.
      taco::AccumulateForce(c1, i, f);
      taco::AccumulateForce(c2, i, -f);
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
  Cell *root =
      taco::PartitionBSP<DIM, real_t, particle, 0>(
          p, np, r, s);
  taco::Map(interact, taco::Product(*root, *root));
  
  //TACO_vec *force = get_force(root);
  //return force;
}
