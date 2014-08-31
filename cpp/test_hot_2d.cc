#include "taco/stdcbug.h"

#include <iostream>
#include <cmath>
#include <cassert>

#include "taco/hot.h"
#include "taco/vec.h"
#include "taco/basic_types.h"

#define TEST_DIM (2)
#include "test_util.h"

using namespace std;
using taco::hot::CalcMortonKey;
using taco::hot::FindFinestAncestor;
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
  Region r(Vec(0.0, 0.0), Vec(1.0, 1.0));
  int np = 100;
  const int max_depth = 2;
  const int depth_bit_width = taco::CalcMinBitLen(max_depth);
  particle *p = GetParticles(np, r);
  PrintParticles<TEST_DIM>(p, 10, std::cout);
  taco::hot::HelperNode<TEST_DIM> *hn =
      taco::hot::CreateInitialNodes<TEST_DIM, real_t, particle, 0>(
          p, np, r, max_depth);
  taco::hot::SortNodes<TEST_DIM>(hn, np);
  PrintHelperNode(hn, 10, std::cout);

  //test_FindFinestAncestor();

  KeyType a = FindFinestAncestor<TEST_DIM>(hn[0].key, hn[np-1].key,
                                           max_depth, depth_bit_width);
  cout << "Finest ancestor: " << a << endl;

  cout << "Keys: " << hn[0].key << "->" << hn[np-1].key << endl;
  taco::hot::KeyVector ks;
  taco::hot::CompleteRegion<TEST_DIM>(hn[0].key, hn[np-1].key, ks, max_depth);
  taco::hot::PrintKeys(ks, cout);
  
  return 0;
}
