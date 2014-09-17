#include "tapas/stdcbug.h"

#include <iostream>
#include <cmath>
#include <cassert>

#include "tapas/hot.h"
#include "tapas/vec.h"
#include "tapas/basic_types.h"

#include "test_util.h"

using namespace std;
using tapas::hot::CalcMortonKey;
using tapas::hot::FindFinestAncestor;
using tapas::hot::KeyType;
using tapas::hot::KeyPair;

typedef tapas::Vec<TEST_DIM, int> VecAnchor;

template <int DIM>
void PrintHelperNode(tapas::hot::HelperNode<DIM> *hn, int n,
                     ostream &os) {
  for (int i = 0; i < n; ++i) {
    os << "Anchor: " << hn[i].anchor << ", key: " << hn[i].key << endl;
  }
}

void test_FindFinestAncestor() {
  const int max_depth = 5;
  const int depth_bit_width = tapas::CalcMinBitLen(max_depth);
  
  KeyType a = FindFinestAncestor<TEST_DIM>(0, 0, max_depth, depth_bit_width);
  assert(a == 0);
  a = FindFinestAncestor<TEST_DIM>(5, 5, max_depth, depth_bit_width);
  assert(a == 5);
  a = FindFinestAncestor<TEST_DIM>((1 << 3) + 5, 5, max_depth, depth_bit_width);
  assert(a == 4);
  a = FindFinestAncestor<TEST_DIM>(4, 3, max_depth, depth_bit_width);
  assert(a == 3);
  a = FindFinestAncestor<TEST_DIM>(5, ((~0) << 3) + 5, max_depth, depth_bit_width);
  assert(a == 0);
  
  VecAnchor a1(1 << (max_depth - 1), 1 << (max_depth - 1), 1 << (max_depth - 1));
  VecAnchor a2((1 << max_depth) - 1, (1 << max_depth) - 1, (1 << max_depth) - 1);
  KeyType a1k = CalcMortonKey<TEST_DIM>(a1, max_depth, depth_bit_width);
  KeyType a2k = CalcMortonKey<TEST_DIM>(a2, max_depth, depth_bit_width);
  a = FindFinestAncestor<TEST_DIM>(a1k, a2k, max_depth, depth_bit_width);
  KeyType b = ((a1k >> 3) << 3) | 1;
  assert(a == b);
}

int main(int argc, char *argv[]) {
  Region r(Vec(0.0, 0.0, 0.0), Vec(1.0, 1.0, 1.0));
  int np = 1000;
  const int max_depth = 5;
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
  PrintParticles<TEST_DIM>(p, 10, std::cout);  


  test_FindFinestAncestor();

  KeyType a = FindFinestAncestor<TEST_DIM>(hn[0].key, hn[np-1].key,
                                           max_depth, depth_bit_width);
  cout << "Finest ancestor: " << a << endl;

  cout << "Keys: " << hn[0].key << "->" << hn[np-1].key << endl;
  tapas::hot::KeyVector ks;
  tapas::hot::CompleteRegion<TEST_DIM>(hn[0].key, hn[np-1].key, ks,
                                      max_depth);
  tapas::hot::PrintKeys(ks, cout);

  KeyPair kp = FindBodyRange(a, hn, p, 0, np, max_depth, depth_bit_width);
  cout << "Root range: offset: " << kp.first << ", length: " << kp.second << "\n";
  
  return 0;
}
