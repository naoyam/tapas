#include <cstdio>
#include "tapas/vec.h"

using namespace tapas;

int main(int argc, char *argv[]) {

  Vec<3, double> x;
  x[0] = 1.0;
  x[1] = 2.0;
  x[2] = 3.0;
  printf("%f\n", x[0]);
  printf("%f\n", x[1]);
  printf("%f\n", x[2]);

  Vec<3, double> y(1.0, 2.0, 3.0);
  printf("%f\n", y[0]);
  printf("%f\n", y[1]);
  printf("%f\n", y[2]);

  Vec<3, double> z(Vec<3, double>(0.0, 1.0, 2.0));
  printf("%f\n", z[0]);
  printf("%f\n", z[1]);
  printf("%f\n", z[2]);
  
  
  return 0;
}
  
  
