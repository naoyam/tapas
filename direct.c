#include <stdio.h>
#include <stdlib.h>
#define DEFAULT_FP_TYPE_DOUBLE
#define DEFAULT_NUM_DIM 3
#include "taco.h"

#pragma taco particle_type(pos.x[0], pos.x[1], pos.x[2])
struct particle {
  vec pos;
  real_t m;
};

struct cell {
};

static void direct(const struct cell *c1, const struct cell *c2) {
  for (int i = 0; i < cell_np(c1); ++i) {
    for (int j = 0; j < cell_np(c2); ++j) {
      struct particle p1 = *(struct particle*)get_particle(c1, i);
      struct particle p2 = *(struct particle*)get_particle(c2, i);
      vec d = vec_sub(p1.pos, p2.pos);
      // By convention, "force" is defined as a vector for each
      // particle. accumulate_force is a special function that
      // accumulates all contributions to the force for a particle.
      accumulate_force(c1, i, vec_mul_s(d, p1.m));
      accumulate_force(c2, i, vec_mul_s(d, -p2.m));
    }
  }
}


// only c1 and c2 can be modified
// c1 and c2 are restrict pointers
static void interact(const struct cell *c1, const struct cell *c2) {
  if (is_leaf(c1) && is_leaf(c2)) {
    direct(c1, c2);
  } else if (is_leaf(c1)) {
    map(interact, product(c1, get_subcells(c2)));
  } else if (is_leaf(c2)) {
    map(interact, product(get_subcells(c1), c2));
  } else {
    map(interact, product(get_subcells(c1), get_subcells(c2)));
  }
}

vec *calc_direct(struct particle *p, size_t np, int s) {
  // partition_bsp is a function that partitions the given set of
  // particles by the binary space partitioning. The result is a
  // octree for 3D particles and a quadtree for 2D particles.
  region3r r = {{{0.0, 0.0, 0.0}}, {{1.0, 1.0, 1.0}}};
  cell *root = partition_bsp(p, np, r, s);
  map(interact, product(root, root));
  vec *force = get_force(root);
  return force;
}
