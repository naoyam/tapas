#ifndef TACO_HOT_
#define TACO_HOT_

#include "taco/stdcbug.h"
#include "taco/common.h"
#include "taco/vec.h"
#include "taco/basic_types.h"

namespace taco {

namespace hot {

typedef int KeyType;

template <int DIM>
struct HelperNode {
  KeyType key; // Morton key
  Vec<DIM, int> anchor;
  index_t p_index;
  index_t np;
};

template <int DIM, class FP, class PT, int OFFSET, int MAX_DEPTH>
HelperNode<DIM> *CreateInitialNodes(const PT *p, index_t np, 
                                    const Region<DIM, FP> &r);

template <int DIM, int MAX_DEPTH>
KeyType CalcMortonKey(const Vec<DIM, int> &anchor);


template <int DIM>
HelperNode<DIM> *SortNodes();

} // namespace hot
} // namespace taco

template <int DIM, int MAX_DEPTH>
taco::hot::KeyType taco::hot::CalcMortonKey(const taco::Vec<DIM, int> &anchor) {
  KeyType k = 0;
  int mask = 1 << (MAX_DEPTH - 1);
  for (int i = 0; i < MAX_DEPTH; ++i) {
    for (int d = 0; d < DIM; ++d) {
      k = (k << 1) | ((anchor[d] & mask) >> (MAX_DEPTH - i - 1));
    }
    mask >>= 1;
  }
  int depth_bit_width = Log2<MAX_DEPTH>::x;  
  k = (k << depth_bit_width) | MAX_DEPTH;
  return k;
}

template <int DIM, class FP, class PT, int OFFSET, int MAX_DEPTH>
taco::hot::HelperNode<DIM> *taco::hot::CreateInitialNodes(
    const PT *p, index_t np,
    const Region<DIM, FP> &r) {

  HelperNode<DIM> *nodes = new HelperNode<DIM>[np];
  FP num_cell = 1 << MAX_DEPTH;
  Vec<DIM, FP> pitch;
  for (int d = 0; d < DIM; ++d) {
    pitch[d] = (r.max()[d] - r.min()[d]) / num_cell;
  }
  for (index_t i = 0; i < np; ++i) {
    HelperNode<DIM> &node = nodes[i];
    node.p_index = i;
    node.np = 1;
    Vec<DIM, FP> off = ParticlePosOffset<0, FP>::template vec<DIM>((const void*)&(p[i]));
    off -= r.min();
    off /= pitch;
    for (int d = 0; d < DIM; ++d) {
      node.anchor[d] = (int)(off[d]);
    }
#ifdef TACO_DEBUG
    assert(node.anchor >= 0);
    assert(node.anchor < (1 << MAX_DEPTH));
#endif // TACO_DEBUG
    
    node.key = CalcMortonKey<DIM, MAX_DEPTH>(node.anchor);
  }
  
  return nodes;
}


#endif // TACO_HOT_
