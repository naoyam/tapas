#include <cmath>
#include <cstdlib>
#include <iomanip>
#include <iostream>
#ifdef USE_PAPI
#include <papi.h>
#endif
#include <sys/time.h>
#include <xmmintrin.h>
#include <fstream>

struct float4 {
  float x;
  float y;
  float z;
  float w;
};

const int N = 1 << 10;
const float OPS = 20. * N * N * 1e-9;
const float EPS2 = 1e-6;

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

void P2Psse(float4 *target, float4 *source, int ni, int nj, float eps2) {
#pragma omp parallel for
  for( int i=0; i<ni; i+=4 ) {
    __m128 ax = _mm_setzero_ps();         // ax = 0
    __m128 ay = _mm_setzero_ps();         // ay = 0
    __m128 az = _mm_setzero_ps();         // az = 0
    __m128 phi = _mm_setzero_ps();        // phi = 0
    __m128 xi = _mm_setr_ps(source[i].x,source[i+1].x,source[i+2].x,source[i+3].x);// xi = target->x
    __m128 yi = _mm_setr_ps(source[i].y,source[i+1].y,source[i+2].y,source[i+3].y);// yi = target->y
    __m128 zi = _mm_setr_ps(source[i].z,source[i+1].z,source[i+2].z,source[i+3].z);// zi = target->z
    __m128 R2 = _mm_load1_ps(&eps2);      // R2 = eps2
    __m128 x2 = _mm_load1_ps(&source[0].x); // x2 = source->x
    x2 = _mm_sub_ps(x2, xi);              // x2 = x2 - xi
    __m128 y2 = _mm_load1_ps(&source[0].y); // y2 = source->y
    y2 = _mm_sub_ps(y2, yi);              // y2 = y2 - yi
    __m128 z2 = _mm_load1_ps(&source[0].z); // z2 = source->z
    z2 = _mm_sub_ps(z2, zi);              // z2 = z2 - zi
    __m128 mj = _mm_load1_ps(&source[0].w); // mj = source->w
    __m128 xj = x2;                       // xj = x2
    x2 = _mm_mul_ps(x2, x2);              // x2 = x2 * x2
    R2 = _mm_add_ps(R2, x2);              // R2 = R2 + x2
    __m128 yj = y2;                       // yj = y2
    y2 = _mm_mul_ps(y2, y2);              // y2 = y2 * y2
    R2 = _mm_add_ps(R2, y2);              // R2 = R2 + y2
    __m128 zj = z2;                       // zj = z2
    z2 = _mm_mul_ps(z2, z2);              // z2 = z2 * z2
    R2 = _mm_add_ps(R2, z2);              // R2 = R2 + z2
    __m128 invR;
    x2 = _mm_load_ps(&source[1].x);       // x2 = next source->x,y,z,w
    y2 = x2;                              // y2 = x2;
    z2 = x2;                              // z2 = x2;
    for( int j=0; j<nj-2; j++ ) {
      invR = _mm_rsqrt_ps(R2);            // invR = rsqrt(R2)       1
      R2 = _mm_load1_ps(&eps2);           // R2 = eps2
      x2 = _mm_shuffle_ps(x2, x2, 0x00);  // x2 = source->x
      x2 = _mm_sub_ps(x2, xi);            // x2 = x2 - xi           2
      y2 = _mm_shuffle_ps(y2, y2, 0x55);  // y2 = source->y
      y2 = _mm_sub_ps(y2, yi);            // y2 = y2 - yi           3
      z2 = _mm_shuffle_ps(z2, z2, 0xaa);  // z2 = source->z
      z2 = _mm_sub_ps(z2, zi);            // z2 = z2 - zi           4
      mj = _mm_mul_ps(mj, invR);          // mj = mj * invR         5
      phi = _mm_add_ps(phi, mj);          // phi = phi + mj         6
      invR = _mm_mul_ps(invR, invR);      // invR = invR * invR     7
      invR = _mm_mul_ps(invR, mj);        // invR = invR * mj       8
      mj = _mm_load_ps(&source[j+1].x);   // mj = source->x,y,z,w
      mj = _mm_shuffle_ps(mj, mj, 0xff);  // mj = source->w
      xj = _mm_mul_ps(xj, invR);          // xj = xj * invR         9
      ax = _mm_add_ps(ax, xj);            // ax = ax + xj          10
      xj = x2;                            // xj = x2
      x2 = _mm_mul_ps(x2, x2);            // x2 = x2 * x2          11
      R2 = _mm_add_ps(R2, x2);            // R2 = R2 + x2          12
      x2 = _mm_load_ps(&source[j+2].x);   // x2 = next source->x,y,z,w
      yj = _mm_mul_ps(yj, invR);          // yj = yj * invR        13
      ay = _mm_add_ps(ay, yj);            // ay = ay + yj          14
      yj = y2;                            // yj = y2
      y2 = _mm_mul_ps(y2, y2);            // y2 = y2 * y2          15
      R2 = _mm_add_ps(R2, y2);            // R2 = R2 + y2          16
      y2 = x2;                            // y2 = x2
      zj = _mm_mul_ps(zj, invR);          // zj = zj * invR        17
      az = _mm_add_ps(az, zj);            // az = az + zj          18
      zj = z2;                            // zj = z2
      z2 = _mm_mul_ps(z2, z2);            // z2 = z2 * z2          19
      R2 = _mm_add_ps(R2, z2);            // R2 = R2 + z2          20
      z2 = x2;                            // z2 = x2
    }
    invR = _mm_rsqrt_ps(R2);              // invR = rsqrt(R2)
    R2 = _mm_load1_ps(&eps2);             // R2 = eps2
    x2 = _mm_shuffle_ps(x2, x2, 0x00);    // x2 = source->x
    x2 = _mm_sub_ps(x2, xi);              // x2 = x2 - xi
    y2 = _mm_shuffle_ps(y2, y2, 0x55);    // y2 = source->y
    y2 = _mm_sub_ps(y2, yi);              // y2 = y2 - yi
    z2 = _mm_shuffle_ps(z2, z2, 0xaa);    // z2 = source->z
    z2 = _mm_sub_ps(z2, zi);              // z2 = z2 - zi
    mj = _mm_mul_ps(mj, invR);            // mj = mj * invR
    phi = _mm_add_ps(phi, mj);            // phi = phi + mj
    invR = _mm_mul_ps(invR, invR);        // invR = invR * invR
    invR = _mm_mul_ps(invR, mj);          // invR = invR * mj
    mj = _mm_load_ps(&source[nj-1].x);    // mj = source->x,y,z,w
    mj = _mm_shuffle_ps(mj, mj, 0xff);    // mj = source->w
    xj = _mm_mul_ps(xj, invR);            // xj = xj * invR
    ax = _mm_add_ps(ax, xj);              // ax = ax + xj
    xj = x2;                              // xj = x2
    x2 = _mm_mul_ps(x2, x2);              // x2 = x2 * x2
    R2 = _mm_add_ps(R2, x2);              // R2 = R2 + x2
    yj = _mm_mul_ps(yj, invR);            // yj = yj * invR
    ay = _mm_add_ps(ay, yj);              // ay = ay + yj
    yj = y2;                              // yj = y2
    y2 = _mm_mul_ps(y2, y2);              // y2 = y2 * y2
    R2 = _mm_add_ps(R2, y2);              // R2 = R2 + y2
    zj = _mm_mul_ps(zj, invR);            // zj = zj * invR
    az = _mm_add_ps(az, zj);              // az = az + zj
    zj = z2;                              // zj = z2
    z2 = _mm_mul_ps(z2, z2);              // z2 = z2 * z2
    R2 = _mm_add_ps(R2, z2);              // R2 = R2 + z2
    invR = _mm_rsqrt_ps(R2);              // invR = rsqrt(R2)
    mj = _mm_mul_ps(mj, invR);            // mj = mj * invR
    phi = _mm_add_ps(phi, mj);            // phi = phi + mj
    invR = _mm_mul_ps(invR, invR);        // invR = invR * invR
    invR = _mm_mul_ps(invR, mj);          // invR = invR * mj
    xj = _mm_mul_ps(xj, invR);            // xj = xj * invR
    ax = _mm_add_ps(ax, xj);              // ax = ax + xj
    yj = _mm_mul_ps(yj, invR);            // yj = yj * invR
    ay = _mm_add_ps(ay, yj);              // ay = ay + yj
    zj = _mm_mul_ps(zj, invR);            // zj = zj * invR
    az = _mm_add_ps(az, zj);              // az = az + zj
    for( int k=0; k<4; k++ ) {
      target[i+k].x = ((float*)&ax)[k];   // target->x = ax
      target[i+k].y = ((float*)&ay)[k];   // target->y = ay
      target[i+k].z = ((float*)&az)[k];   // target->z = az
      target[i+k].w = ((float*)&phi)[k];  // target->w = phi
    }
  }
}


int main() {
// ALLOCATE
  float4 *sourceHost = new float4 [N];
  float4 *targetHost = new float4 [N];
  float4 *targetSSE = new float4 [N];
  for( int i=0; i<N; i++ ) {
    sourceHost[i].x = drand48();
    sourceHost[i].y = drand48();
    sourceHost[i].z = drand48();
    sourceHost[i].w = drand48() / N;
  }
  std::cout << std::scientific << "N      : " << N << std::endl;

// Host P2P
#ifdef USE_PAPI
  int Events[3] = { PAPI_L2_DCM, PAPI_L2_DCA, PAPI_TLB_DM };
  int EventSet = PAPI_NULL;
  PAPI_library_init(PAPI_VER_CURRENT);
  PAPI_create_eventset(&EventSet);
  PAPI_add_events(EventSet, Events, 3);
  PAPI_start(EventSet);
#endif

  double tic = get_time();
  P2P(targetHost,sourceHost,N,N,EPS2);
  double toc = get_time();

#ifdef USE_PAPI
  long long values[3];
  PAPI_stop(EventSet,values);
  std::cout << "L2 Miss: " << values[0]
            << " L2 Access: " << values[1]
            << " TLB Miss: " << values[2] << std::endl;
#endif

  std::cout << std::scientific << "No SSE : " << toc-tic << " s : " << OPS / (toc-tic) << " GFlops" << std::endl;

// SSE P2P
#ifdef USE_PAPI
  PAPI_start(EventSet);
#endif
  tic = get_time();
  P2Psse(targetSSE,sourceHost,N,N,EPS2);
  toc = get_time();
#ifdef USE_PAPI
  PAPI_stop(EventSet,values);
  std::cout << "L2 Miss: " << values[0]
            << " L2 Access: " << values[1]
            << " TLB Miss: " << values[2] << std::endl;
#endif

  std::cout << std::scientific << "SSE    : " << toc-tic << " s : " << OPS / (toc-tic) << " GFlops" << std::endl;

#ifdef DUMP
  std::ofstream dout("target.txt");
#endif

// COMPARE RESULTS
  float pd = 0, pn = 0, fd = 0, fn = 0;
  for( int i=0; i<N; i++ ) {
    targetHost[i].w -= sourceHost[i].w / sqrtf(EPS2);
    targetSSE[i].w -= sourceHost[i].w / sqrtf(EPS2);
    pd += (targetHost[i].w - targetSSE[i].w) * (targetHost[i].w - targetSSE[i].w);
    pn += targetHost[i].w * targetHost[i].w;
    fd += (targetHost[i].x - targetSSE[i].x) * (targetHost[i].x - targetSSE[i].x)
        + (targetHost[i].y - targetSSE[i].y) * (targetHost[i].y - targetSSE[i].y)
        + (targetHost[i].z - targetSSE[i].z) * (targetHost[i].z - targetSSE[i].z);
    fn += targetHost[i].x * targetHost[i].x + targetHost[i].y * targetHost[i].y + targetHost[i].z * targetHost[i].z;
#ifdef DUMP
    dout << targetHost[i].x << " " << targetHost[i].y << " "
         << targetHost[i].z << " " << targetHost[i].w << std::endl;
#endif
  }
  std::cout << std::scientific << "P ERR  : " << sqrtf(pd/pn) << std::endl;
  std::cout << std::scientific << "F ERR  : " << sqrtf(fd/fn) << std::endl;

// DEALLOCATE
  delete[] sourceHost;
  delete[] targetHost;
  delete[] targetSSE;
}
