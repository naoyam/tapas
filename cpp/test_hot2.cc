#include "tapas/stdcbug.h"

#include <iostream>
#include <cmath>
#include <cassert>

#include "tapas.h"

#include "tapas/logging.h"

#define TEST_DIM (2)
#include "test_util.h"

using namespace std;

typedef tapas::BodyInfo<particle, 0> BodyInfo;
typedef tapas::Tapas<TEST_DIM, real_t, BodyInfo,
                     particle, tapas::NONE, tapas::HOT> Tapas;

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
  const int max_depth = 3;
  const int max_nb = 10;
  //const int depth_bit_width = tapas::CalcMinBitLen(max_depth);
  particle *p = GetParticles(np, r);
  particle *pb = new particle[np];  
  PrintParticles<TEST_DIM>(p, 10, std::cout);
  tapas::hot::HelperNode<TEST_DIM> *hn =
      tapas::hot::CreateInitialNodes<TEST_DIM, real_t, BodyInfo>(
          p, np, r, max_depth);
  tapas::hot::SortNodes<TEST_DIM>(hn, np);
  std::cout << "Sorted nodes\n";
  PrintHelperNode(hn, 10, std::cout);
  tapas::hot::SortBodies<TEST_DIM, BodyInfo>(p, pb, hn, np);
  std::swap(p, pb);  
  std::cout << "Sorted bodies\n";  
  PrintParticles<TEST_DIM>(p, 100, std::cout);  

  TAPAS_LOG_DEBUG() << "Keys: " << hn[0].key << "->" << hn[np-1].key << endl;
  
  Tapas::Cell *root = Tapas::Partition(max_nb, max_depth, p, np, r);
  
  return 0;
}
