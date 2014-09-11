#include <cstdio>
#include <cstdlib>
#include <cmath>

#include "taco/taco.h"

#define DIM (3)
typedef double real_t;
typedef taco::Vec<DIM, real_t> Vec;
typedef taco::Region<DIM, real_t> Region;

struct particle {
  Vec X;
  real_t m;
};

struct CellAttr {
  Vec center;
  real_t m;
};

typedef taco::Cell<DIM, real_t, particle, 0, particle, CellAttr> Cell;

static Vec ComputeForce(const Vec &p1, real_t m1,
                        const Vec &p2, real_t m2) {
  real_t R2 = ((p1 - p2) * (p1 - p2)).reduce_sum();
  if (R2 == 0) return Vec(0, 0, 0);
  real_t invR2 = 1.0 / R2;
  real_t invR = std::sqrt(invR2);
  real_t invR3 = invR2 * invR * m1 * m2;
  Vec f1 = (p2 - p1) * invR3;
  return f1;
}

static void approximate(Cell &c) {
  if (c.np() == 0) {
    c.attr().m = 0.0;
  } else if (c.np() == 1) {
    c.attr().m = c.particle(0).m;
    c.attr().center = c.particle(0).X;
  } else {
    taco::Map(approximate, c.subcells());
    Vec center(0, 0, 0);
    real_t m = 0;
    for (int i = 0; i < c.nsubcells(); ++i) {
      Cell &sc = c.subcell(i);
      m += sc.attr().m;
      center += sc.attr().center * sc.attr().m;
    }
    c.attr().m = m;
  }
}

static void interact(Cell &c1, Cell &c2, real_t theta) {
  if (c1.np() == 0 || c2.np() == 0) {
    return;
  } else if (!c1.IsLeaf()) {
    taco::Map(interact, taco::Product(c1.subcells(), c2), theta);
  } else if (c2.IsLeaf()) {
    // c1 and c2 have only one particle each. Calculate direct force.
    c1.attr(0).X += ComputeForce(c1.particle(0).X, c1.particle(0).m,
                                 c2.particle(0).X, c2.particle(0).m);
  } else {
    // use apploximation
    const particle &p1 = c1.particle(0);
    Vec c2center = c2.attr().center;
    real_t d = std::sqrt(((p1.X - c2center) * (p1.X - c2center)).reduce_sum());
    real_t s = c2.width(0);
    if ((s / d) < theta) {
      c1.attr(0).X += ComputeForce(p1.X, p1.m, c2center, c2.attr().m);
    } else {
      taco::Map(interact, taco::Product(c1, c2.subcells()), theta);
    }
  }
}


particle *calc(struct particle *p, size_t np, int s) {
  // PartitionBSP is a function that partitions the given set of
  // particles by the binary space partitioning. The result is a
  // octree for 3D particles and a quadtree for 2D particles.
  Region r(Vec(0.0, 0.0, 0.0), Vec(1.0, 1.0, 1.0));
  //Cell *root = taco::PartitionBSP<particle, Region, Cell>(p, np, r,
  //s);
  Cell *root = taco::PartitionBSP<DIM, real_t, particle, 0,
                                  particle, CellAttr>(
      p, np, r, s);
  taco::Map(approximate, *root);
  real_t theta = 0.5;
  particle *out = taco::Map(interact, taco::Product(*root, *root), theta);
  return out;
}
