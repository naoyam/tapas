#include <stdio.h>
#include <stdlib.h>

#define DEFAULT_FP_TYPE_DOUBLE
#define DEFAULT_NUM_DIM 3
#include "taco/vec.h"

#pragma taco particle_type(pos.x[0], pos.x[1], pos.x[2])
typedef struct particle {
  TACO_vec pos;
  TACO_real_t m;
} particle;

typedef struct cell {
  TACO_real_t m; // multipole expansion
  TACO_real_t l; // local expansion
} cell;

// Specify user-defined types
#define PARTICLE_TYPE particle
#define CELL_TYPE cell
#include "taco/taco.h"

static void direct(const cell * const c1, const cell * const c2) {
  for (int i = 0; i < cell_np(c1); ++i) {
    for (int j = 0; j < cell_np(c2); ++j) {
      particle p1 = get_particle(c1, i);
      particle p2 = get_particle(c2, i);
      TACO_vec d = TACO_vec_sub(p1.pos, p2.pos);
      // By convention, "force" is defined as a vector for each
      // particle. accumulate_force is a special function that
      // accumulates all contributions to the force for a particle.
      accumulate_force(c1, i, TACO_vec_mul_s(d, p1.m));
      accumulate_force(c2, i, TACO_vec_mul_s(d, -p2.m));
    }
  }
}

// dummy
static TACO_real_t p2m(const cell * const c) {
  return 0.0; 
}

// dummy; just averaging multipole of sub cells
static TACO_real_t m2m(cell * const c) {
  TACO_real_t m = 0.0;
  for (int i = 0; i < 8; ++i) {
    m += get_subcell(c, i)->m;
  }
  m /= 8.0;
  return m;
}

static void fmm_p2m(cell * const c) {
  if (is_leaf(c)) {
    c->m = p2m(c);
  } else {
    map(fmm_p2m, get_subcells(c));
    c->m = m2m(c);
  }
}

// dummy
static void l2l(cell * const c) {
  cell *p = get_parent(c);
  c->l = p->l;
  return;
}

static void l2p(cell * const c) {
  for (int i = 0; i < cell_np(c); ++i) {
    particle p = get_particle(c, i);
    // compute by using c->l 
  }
}

static void fmm_l2l(cell * const c) {
  if (!is_root(c)) {
    l2l(c);
  }
  if (is_leaf(c)) {
    l2p(c);
  } else {
    map(fmm_l2l, get_subcells(c));
  }
}

static TACO_real_t m2l(const cell * const c1, const cell * const c2) {
  return 0.0;
}

static void fmm_m2l(cell * const c1, cell * const c2) {
  cell *gp1 = get_grand_parent(c1);
  cell *gp2 = get_grand_parent(c2);
  if (gp1 != NULL && gp2 != NULL &&
      gp1 == gp2) {
    TACO_real_t l = m2l(c1, c2);
    c1->l += l;
    c2->l += -l;
  } else if (c1 == c2 || get_parent(c1) == get_parent(c2)) {
    map(fmm_m2l, product(get_subcells(c1), get_subcells(c2)));
  }
}

// only c1 and c2 can be modified
// c1 and c2 are restrict pointers
static void fmm_p2p(cell * const c1, cell * const c2,
                    TACO_real_t p2p_cutoff) {
  if (cell_distance(c1, c2) < p2p_cutoff) {
    if (is_leaf(c1) && is_leaf(c2)) {
      direct(c1, c2);
    } else if (is_leaf(c1)) {
      map(fmm_p2p, product(c1, get_subcells(c2)), p2p_cutoff);
    } else if (is_leaf(c2)) {
      map(fmm_p2p, product(get_subcells(c1), c2), p2p_cutoff);
    } else {
      map(fmm_p2p, product(get_subcells(c1), get_subcells(c2)), p2p_cutoff);
    }
  }
}

TACO_vec *calc_fmm(particle *p, size_t np, int s, TACO_real_t p2p_cutoff) {
  // partition_bsp is a function that partitions the given set of
  // particles by the binary space partitioning. The result is a
  // octree for 3D particles and a quadtree for 2D particles.
  TACO_region3r r = {{{0.0, 0.0, 0.0}}, {{1.0, 1.0, 1.0}}};
  cell *root = partition_bsp(p, np, r, s);
  map(fmm_p2m, root);
  map(fmm_m2l, product(root, root));
  map(fmm_l2l, root);
  map(fmm_p2p, product(root, root), p2p_cutoff);    
  TACO_vec *force = get_force(root);
  return force;
}
