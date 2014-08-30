#include "taco/stdcbug.h"

#include <iostream>
#include <cmath>
#include <cassert>

#include "taco/hot.h"
#include "taco/vec.h"
#include "taco/basic_types.h"

#include "test_util.h"

using namespace std;
using taco::hot::CalcMortonKey;
using taco::hot::FinestAncestor;
using taco::hot::KeyType;

typedef taco::Vec<TEST_DIM, int> VecAnchor;

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
  const int max_depth = 5;
  particle *p = GetParticles(np, r);
  PrintParticles<TEST_DIM>(p, 10, std::cout);
  taco::hot::HelperNode<TEST_DIM> *hn =
      taco::hot::CreateInitialNodes<TEST_DIM, real_t, particle, 0, max_depth>(
          p, np, r);
  taco::hot::SortNodes<TEST_DIM>(hn, np);
  PrintHelperNode(hn, 10, std::cout);

  KeyType a = FinestAncestor<TEST_DIM, max_depth>(0, 0);
  assert(a == 0);
  a = FinestAncestor<TEST_DIM, max_depth>(5, 5);
  assert(a == 5);
  a = FinestAncestor<TEST_DIM, max_depth>((1 << 3) + 5, 5);
  assert(a == 4);
  a = FinestAncestor<TEST_DIM, max_depth>(4, 3);
  assert(a == 3);
  a = FinestAncestor<TEST_DIM, max_depth>(5, ((~0) << 3) + 5);
  assert(a == 0);
  
  VecAnchor a1(1 << (max_depth - 1), 1 << (max_depth - 1), 1 << (max_depth - 1));
  VecAnchor a2((1 << max_depth) - 1, (1 << max_depth) - 1, (1 << max_depth) - 1);
  KeyType a1k = CalcMortonKey<TEST_DIM, max_depth>(a1);
  KeyType a2k = CalcMortonKey<TEST_DIM, max_depth>(a2);
  a = FinestAncestor<TEST_DIM, max_depth>(a1k, a2k);
  KeyType b = ((a1k >> 3) << 3) | 1;
  assert(a == b);
  
  return 0;
}
