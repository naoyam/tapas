#include "taco/stdcbug.h"

#include <iostream>
#include <cmath>

#include "taco/hot.h"
#include "taco/vec.h"
#include "taco/basic_types.h"

#include "test_util.h"

using namespace std;

template <int DIM>
void PrintHelperNode(taco::hot::HelperNode<DIM> *hn, int n,
                     ostream &os) {
  for (int i = 0; i < n; ++i) {
    os << "Anchor: " << hn[i].anchor << ", key: " << hn[i].key << endl;
  }
}

int main(int argc, char *argv[]) {
  Region r(Vec(0.0, 0.0, 0.0), Vec(1.0, 1.0, 1.0));
  int np = 1000;
  int max_depth = 5;

  particle *p = GetParticles(np, r);
  PrintParticles<TEST_DIM>(p, 10, std::cout);
  taco::hot::HelperNode<TEST_DIM> *hn =
      taco::hot::CreateInitialNodes<TEST_DIM, real_t, particle, 0>(p, np, r, max_depth);
  PrintHelperNode(hn, 10, std::cout);
  return 0;
}
