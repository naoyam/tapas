#include <cstdio>
#include <cstdlib>
#include <cmath>

#include "tapas.h"

#define DIM (3)
typedef double real_t;
typedef tapas::Vec<DIM, real_t> Vec;
typedef tapas::Region<DIM, real_t> Region;

struct body {
  Vec X;
  real_t m;
};

struct CellAttr {
  Vec center;
  real_t m;
};

typedef tapas::BodyInfo<body, 0> BodyInfo;
typedef tapas::Tapas<DIM, real_t, BodyInfo,
                     body, CellAttr, tapas::HOT> Tapas;


static void ComputeForce(Tapas::BodyIterator &p1, 
                         Vec center, real_t m) {
  real_t R2 = ((p1->X - center) * (p1->X - center)).reduce_sum();
  if (R2 != 0) {
    real_t invR2 = 1.0 / R2;
    real_t invR = std::sqrt(invR2);
    real_t invR3 = invR2 * invR * p1->m * m;
    p1.attr().X += (center - p1->X) * invR3;
  }
}

static void approximate(Tapas::Cell &c) {
  if (c.nb() == 0) {
    c.attr().m = 0.0;
  } else if (c.nb() == 1) {
    c.attr().m = c.body(0).m;
    c.attr().center = c.body(0).X;
  } else {
    tapas::Map(approximate, c.subcells());
    Vec center(0, 0, 0);
    real_t m = 0;
    for (int i = 0; i < c.nsubcells(); ++i) {
      Tapas::Cell &sc = c.subcell(i);
      m += sc.attr().m;
      center += sc.attr().center * sc.attr().m;
    }
    c.attr().m = m;
  }
}

static void interact(Tapas::Cell &c1, Tapas::Cell &c2, real_t theta) {
  if (c1.nb() == 0 || c2.nb() == 0) {
    return;
  } else if (!c1.IsLeaf()) {
    tapas::Map(interact, tapas::Product(c1.subcells(), c2), theta);
  } else if (c2.IsLeaf()) {
    // c1 and c2 have only one particle each. Calculate direct force.
    //tapas::Map(ComputeForce, tapas::Product(c1.particles(),
    //c2.particles()));
    tapas::Map(ComputeForce, c1.bodies(), c2.body(0).X,
               c2.body(0).m);
  } else {
    // use apploximation
    const body &p1 = c1.body(0);
    Vec c2center = c2.attr().center;
    real_t d = std::sqrt(((p1.X - c2center) * (p1.X - c2center)).reduce_sum());
    real_t s = c2.width(0);
    if ((s / d) < theta) {
      tapas::Map(ComputeForce, c1.bodies(), c2center, c2.attr().m);
    } else {
      tapas::Map(interact, tapas::Product(c1, c2.subcells()), theta);
    }
  }
}


body *calc(body *p, size_t np) {
  // PartitionBSP is a function that partitions the given set of
  // particles by the binary space partitioning. The result is a
  // octree for 3D particles and a quadtree for 2D particles.
  Region r(Vec(0.0, 0.0, 0.0), Vec(1.0, 1.0, 1.0));
  //Cell *root = tapas::PartitionBSP<particle, Region, Cell>(p, np, r,
  //s);
  Tapas::Cell *root = Tapas::Partition(p, np, r, 1);
  tapas::Map(approximate, *root);
  real_t theta = 0.5;
  tapas::Map(interact, tapas::Product(*root, *root), theta);
  body *out = root->body_attrs();
  return out;
}
