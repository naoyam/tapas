#include <cmath>
#include <cstdlib>
#include <iomanip>
#include <iostream>
#include <sys/time.h>
#include <fstream>
#include <cstdio>

#include "tapas.h"

#define DIM (3)

struct float4 {
  float x;
  float y;
  float z;
  float w;
};

#ifdef NB
const int N = NB;
#else
const int N = 1 << 10;
#endif
const float OPS = 20. * N * N * 1e-9;
const float EPS2 = 1e-6;

#ifndef S
#define S (10)
#endif

double get_time() {
  struct timeval tv;
  gettimeofday(&tv,NULL);
  return double(tv.tv_sec+tv.tv_usec*1e-6);
}

void P2P(float4 *target, float4 *source, int ni, int nj, float eps2) {
#pragma omp parallel for
  for (int i=0; i<ni; i++) {
    float ax = 0;
    float ay = 0;
    float az = 0;
    float phi = 0;
    float xi = source[i].x;
    float yi = source[i].y;
    float zi = source[i].z;
    for (int j=0; j<nj; j++) {
      float dx = source[j].x - xi;
      float dy = source[j].y - yi;
      float dz = source[j].z - zi;
      float R2 = dx * dx + dy * dy + dz * dz + eps2;
      float invR = 1.0f / sqrtf(R2);
      float invR3 = source[j].w * invR * invR * invR;
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

typedef tapas::BodyInfo<float4, 0> BodyInfo;
typedef tapas::Tapas<DIM, float, BodyInfo,
                     float4, tapas::NONE, tapas::HOT> Tapas;

static void direct(Tapas::BodyIterator &p1, Tapas::BodyIterator &p2, float eps2) {
  float dx = p2->x - p1->x;
  std::cerr << "x: " << p1->x << ", " << p2->x << std::endl;
  float dy = p2->y - p1->y;
  float dz = p2->z - p1->z;
  float R2 = dx * dx + dy * dy + dz * dz + eps2;
  float invR = 1.0f / sqrtf(R2);
  float invR3 = invR * invR * invR;
  p1.attr().x += dx * invR3 * p2->w;
  p1.attr().y += dy * invR3 * p2->w;
  p1.attr().z += dz * invR3 * p2->w;
  p1.attr().w += invR * p2->w;
  if (p1 != p2) {
    p2.attr().x += -dx * invR3 * p1->w;
    p2.attr().y += -dy * invR3 * p1->w;
    p2.attr().z += -dz * invR3 * p1->w;
    p2.attr().w += invR * p1->w;
  }
}
// only c1 and c2 can be modified
static void interact(Tapas::Cell &c1, Tapas::Cell &c2) {
  if (c1.IsLeaf() && c2.IsLeaf()) {
    tapas::Map(direct, tapas::Product(c1.bodies(), c2.bodies()), EPS2);
  } else if (c1.IsLeaf()) {
    tapas::Map(interact, tapas::Product(c1, c2.subcells()));
  } else if (c2.IsLeaf()) {
    tapas::Map(interact, tapas::Product(c1.subcells(), c2));
  } else {
    tapas::Map(interact, tapas::Product(c1.subcells(), c2.subcells()));
  }
}

float4 *calc_direct(float4 *p, size_t np, int s) {
  // PartitionBSP is a function that partitions the given set of
  // particles by the binary space partitioning. The result is a
  // octree for 3D particles and a quadtree for 2D particles.
  tapas::Region<3, float> r(tapas::Vec<3, float>(0.0, 0.0, 0.0), tapas::Vec<3, float>(1.0, 1.0, 1.0));
  Tapas::Cell *root = Tapas::Partition(p, np, r, s);
  tapas::Map(interact, tapas::Product(*root, *root));
  return root->body_attrs();
}

int main() {
// ALLOCATE
  float4 *sourceHost = new float4 [N];
  float4 *targetHost = new float4 [N];
  //float4 *targetSSE = new float4 [N];
  srand48(0);
  for( int i=0; i<N; i++ ) {
    sourceHost[i].x = drand48();
    sourceHost[i].y = drand48();
    sourceHost[i].z = drand48();
    sourceHost[i].w = drand48() / N;
  }
  std::cout << std::scientific << "N      : " << N << std::endl;


  float4 *targetTapas = calc_direct(sourceHost, N, S);
#ifdef DUMP
  std::ofstream ref_out("ref.txt");
  std::ofstream tapas_out("tapas.txt");
#endif

  double tic = get_time();
  P2P(targetHost,sourceHost,N,N,EPS2);
  double toc = get_time();

  std::cout << std::scientific << "No SSE : " << toc-tic << " s : " << OPS / (toc-tic) << " GFlops" << std::endl;

// COMPARE RESULTS
  float pd = 0, pn = 0, fd = 0, fn = 0;
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
