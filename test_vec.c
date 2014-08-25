#include <stdio.h>
#define DEFAULT_FP_TYPE_DOUBLE
#include "taco/vec.h"

int main(int argc, char *argv[]) {

  TACO_vec3r x = {{0.0, 1.0, 2.0}};
  TACO_vec3r y = TACO_vec3r_mul_s(x, 2.0);
  printf("%f\n", y.x[0]);
  printf("%f\n", y.x[1]);
  printf("%f\n", y.x[2]);
  return 0;
}
  
  
