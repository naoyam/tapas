#include "taco/stdcbug.h"

#include <cstdio>
#include <iostream>

#include "taco/taco.h"

using namespace taco;

int main(int argc, char *argv[]) {

  Region<3, double> r(Vec<3, double>(0.0, 0.0, 0.0),
                      Vec<3, double>(1.0, 1.0, 1.0));

  for (int i = 0; i < 3; ++i) {
    std::cout << r.min()[i] << ", " << r.max()[i] << std::endl;
  }
  
  return 0;
}
  
  
