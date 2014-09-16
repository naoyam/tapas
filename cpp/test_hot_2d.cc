#include "tapas/stdcbug.h"

#include <iostream>
#include <cmath>
#include <cassert>

#include "tapas/hot.h"
#include "tapas/vec.h"
#include "tapas/basic_types.h"

#define TEST_DIM (2)
#include "test_util.h"

using namespace std;
using tapas::hot::CalcMortonKey;
using tapas::hot::FindFinestAncestor;
using tapas::hot::KeyType;

typedef tapas::Vec<TEST_DIM, int> VecAnchor;

template <int DIM>
void PrintHelperNode(tapas::hot::HelperNode<DIM> *hn, int n,
                     ostream &os) {
  for (int i = 0; i < n; ++i) {
    os << "Anchor: " << hn[i].anchor << ", key: " << hn[i].key << endl;
  }
}

int main(int argc, char *argv[]) {
  Region r(Vec(0.0, 0.0), Vec(1.0, 1.0));
  int np = 100;
  const int max_depth = 2;
  const int depth_bit_width = tapas::CalcMinBitLen(max_depth);
  particle *p = GetParticles(np, r);
  particle *pb = new particle[np];  
  PrintParticles<TEST_DIM>(p, 10, std::cout);
  tapas::hot::HelperNode<TEST_DIM> *hn =
      tapas::hot::CreateInitialNodes<TEST_DIM, real_t, particle, 0>(
          p, np, r, max_depth);
  tapas::hot::SortNodes<TEST_DIM>(hn, np);
  std::cout << "Sorted nodes\n";
  PrintHelperNode(hn, 10, std::cout);
  tapas::hot::SortBodies<TEST_DIM, particle>(p, pb, hn, np);
  std::swap(p, pb);  
  std::cout << "Sorted bodies\n";  
  PrintParticles<TEST_DIM>(p, 100, std::cout);  
  

  //test_FindFinestAncestor();

  KeyType a = FindFinestAncestor<TEST_DIM>(hn[0].key, hn[np-1].key,
                                           max_depth, depth_bit_width);
  cout << "Finest ancestor: " << a << endl;

  cout << "Keys: " << hn[0].key << "->" << hn[np-1].key << endl;
  tapas::hot::KeyVector ks;
  tapas::hot::CompleteRegion<TEST_DIM>(hn[0].key, hn[np-1].key, ks, max_depth);
  tapas::hot::PrintKeys(ks, cout);
  
  return 0;
}
