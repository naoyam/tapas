#include <cstdio>
#include <cstdlib>
#include <cmath>
#include <iostream>
#include <fstream>
#include <sys/time.h>

#include "tapas.h"

#define DIM (3)
typedef double real_t;

struct float4 {
  real_t x;
  real_t y;
  real_t z;
  real_t w;
};

#ifdef NB
const int N = NB;
#else
const int N = 1 << 10;
#endif
const real_t OPS = 20. * N * N * 1e-9;
const real_t EPS2 = 1e-6;

typedef tapas::BodyInfo<float4, 0> BodyInfo;
typedef tapas::Tapas<DIM, real_t, BodyInfo,
        float4, float4, tapas::HOT> Tapas;

double get_time() {
  struct timeval tv;
  gettimeofday(&tv,NULL);
  return double(tv.tv_sec+tv.tv_usec*1e-6);
}


void P2P(float4 *target, float4 *source, int ni, int nj, float eps2) {
#pragma omp parallel for
  for (int i=0; i<ni; i++) {
    real_t ax = 0;
    real_t ay = 0;
    real_t az = 0;
    real_t phi = 0;
    real_t xi = source[i].x;
    real_t yi = source[i].y;
    real_t zi = source[i].z;
    for (int j=0; j<nj; j++) {
      real_t dx = source[j].x - xi;
      real_t dy = source[j].y - yi;
      real_t dz = source[j].z - zi;
      real_t R2 = dx * dx + dy * dy + dz * dz + eps2;
      real_t invR = 1.0f / std::sqrt(R2);
      real_t invR3 = source[j].w * invR * invR * invR;
      phi += source[j].w * invR;
      ax += dx * invR3;
      ay += dy * invR3;
      az += dz * invR3;
    }
    target[i].w = phi;
    target[i].x = ax;
    target[i].y = ay;
    target[i].z = az;
  }
}

static real_t distR2(const float4 &p, const float4 &q) {
  real_t dx = q.x - p.x;
  real_t dy = q.y - p.y;
  real_t dz = q.z - p.z;
  return dx * dx + dy * dy + dz * dz;
}


static void ComputeForce(Tapas::BodyIterator &p1, 
                         float4 approx, real_t eps2) {
  real_t dx = approx.x - p1->x;
  real_t dy = approx.y - p1->y;
  real_t dz = approx.z - p1->z;
  real_t R2 = dx * dx + dy * dy + dz * dz + eps2;
  real_t invR = 1.0 / std::sqrt(R2);
  real_t invR3 = invR * invR * invR;
  p1.attr().x += dx * invR3 * approx.w;
  p1.attr().y += dy * invR3 * approx.w;
  p1.attr().z += dz * invR3 * approx.w;
  p1.attr().w += invR * approx.w;
}

static void approximate(Tapas::Cell &c) {
  std::cerr << "Approximate: " << c.key() << std::endl;
  if (c.nb() == 0) {
    c.attr().w = 0.0;
#if 1    
    c.attr().x = 0.0;
    c.attr().y = 0.0;
    c.attr().z = 0.0;
#endif
    std::cerr << "Empty" << std::endl;
  } else if (c.nb() == 1) {
    c.attr() = c.body(0);
    std::cerr << "One particle" << std::endl;
  } else {
    tapas::Map(approximate, c.subcells());
    float4 center = {0.0, 0.0, 0.0, 0.0};
    for (int i = 0; i < c.nsubcells(); ++i) {
      Tapas::Cell &sc = c.subcell(i);
      center.w += sc.attr().w;
      center.x += sc.attr().x * sc.attr().w;
      center.y += sc.attr().y * sc.attr().w;
      center.z += sc.attr().z * sc.attr().w;
    }
    center.x /= center.w;
    center.y /= center.w;
    center.z /= center.w;
    c.attr() = center;
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
    tapas::Map(ComputeForce, c1.bodies(), c2.body(0), EPS2);
  } else {
    // use apploximation
    const float4 &p1 = c1.body(0);
    std::cerr << "c2: " << c2.key() << std::endl;
    std::cerr << "r2: " << distR2(c2.attr(), p1) << std::endl;
    real_t d = std::sqrt(distR2(c2.attr(), p1));
    std::cerr << "c2 region: " << c2.region() << std::endl;
    real_t s = c2.width(0);
#ifdef DISABLE_APPROXIMATION
    tapas::Map(interact, tapas::Product(c1, c2.subcells()), theta);
#else
    //if ((s / d) < theta) {
    if ((s/ d) < theta) {
      tapas::Map(ComputeForce, c1.bodies(), c2.attr(), EPS2);
    } else {
      tapas::Map(interact, tapas::Product(c1, c2.subcells()), theta);
    }
#endif    
  }
}

float4 *calc(float4 *p, size_t np) {
  // PartitionBSP is a function that partitions the given set of
  // particles by the binary space partitioning. The result is a
  // octree for 3D particles and a quadtree for 2D particles.
  Tapas::Region r(Tapas::Vec3(0.0, 0.0, 0.0), Tapas::Vec3(1.0, 1.0, 1.0));
  Tapas::Cell *root = Tapas::Partition(p, np, r, 1);
  tapas::Map(approximate, *root); // or, simply: approximate(*root);
  real_t theta = 0.5;
  tapas::Map(interact, tapas::Product(*root, *root), theta);
  float4 *out = root->body_attrs();
  return out;
}

int main() {
// ALLOCATE
  float4 *sourceHost = new float4 [N];
  float4 *targetHost = new float4 [N];
  srand48(0);
  for( int i=0; i<N; i++ ) {
    sourceHost[i].x = drand48();
    sourceHost[i].y = drand48();
    sourceHost[i].z = drand48();
    sourceHost[i].w = drand48() / N;
  }
  std::cout << std::scientific << "N      : " << N << std::endl;

  float4 *targetTapas = calc(sourceHost, N);

  double tic = get_time();
  P2P(targetHost,sourceHost,N,N,EPS2);
  double toc = get_time();

#ifdef DUMP
  std::ofstream ref_out("bh_ref.txt");
  std::ofstream tapas_out("bh_tapas.txt");
#endif

  std::cout << std::scientific << "No SSE : " << toc-tic << " s : " << OPS / (toc-tic) << " GFlops" << std::endl;

// COMPARE RESULTS
  real_t pd = 0, pn = 0, fd = 0, fn = 0;
  for( int i=0; i<N; i++ ) {
#ifdef DUMP
    ref_out << targetHost[i].x << " " << targetHost[i].y << " "
            << targetHost[i].z << " " << targetHost[i].w << std::endl;
    tapas_out << targetTapas[i].x << " " << targetTapas[i].y << " "
              << targetTapas[i].z << " " << targetTapas[i].w << std::endl;
#endif
    targetHost[i].w -= sourceHost[i].w / sqrtf(EPS2);
    targetTapas[i].w -= sourceHost[i].w / sqrtf(EPS2);
    pd += (targetHost[i].w - targetTapas[i].w) * (targetHost[i].w - targetTapas[i].w);
    pn += targetHost[i].w * targetHost[i].w;
    fd += (targetHost[i].x - targetTapas[i].x) * (targetHost[i].x - targetTapas[i].x)
        + (targetHost[i].y - targetTapas[i].y) * (targetHost[i].y - targetTapas[i].y)
        + (targetHost[i].z - targetTapas[i].z) * (targetHost[i].z - targetTapas[i].z);
    fn += targetHost[i].x * targetHost[i].x + targetHost[i].y * targetHost[i].y + targetHost[i].z * targetHost[i].z;
  }
  std::cout << std::scientific << "P ERR  : " << sqrtf(pd/pn) << std::endl;
  std::cout << std::scientific << "F ERR  : " << sqrtf(fd/fn) << std::endl;

// DEALLOCATE
  delete[] sourceHost;
  delete[] targetHost;
  //delete[] targetTapas;
}
