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

template <int DIM, class FP, class PT, int OFFSET>
HelperNode<DIM> *CreateInitialNodes(const PT *p, index_t np, 
                                    const Region<DIM, FP> &r, int max_depth);

template <int DIM>
KeyType CalcMortonKey(const Vec<DIM, int> &anchor, int max_depth,
                      int depth_bit_width);

} // namespace hot
} // namespace taco

template <int DIM>
taco::hot::KeyType taco::hot::CalcMortonKey(const taco::Vec<DIM, int> &anchor,
                                            int max_depth, int depth_bit_width) {
  KeyType k = 0;
  int mask = 1 << (max_depth - 1);
  for (int i = 0; i < max_depth; ++i) {
    for (int d = 0; d < DIM; ++d) {
      k = (k << 1) | ((anchor[d] & mask) >> (max_depth - i - 1));
    }
    mask >>= 1;
  }
  k = (k << depth_bit_width) | max_depth;
  return k;
}

template <int DIM, class FP, class PT, int OFFSET>
taco::hot::HelperNode<DIM> *taco::hot::CreateInitialNodes(
    const PT *p, index_t np,
    const Region<DIM, FP> &r, int max_depth) {

  HelperNode<DIM> *nodes = new HelperNode<DIM>[np];
  FP num_cell = 1 << max_depth;
  Vec<DIM, FP> pitch;
  for (int d = 0; d < DIM; ++d) {
    pitch[d] = (r.max()[d] - r.min()[d]) / num_cell;
  }
  int depth_bit_width = std::floor(std::log2(max_depth)) + 1;
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
    assert(node.anchor < (1 << max_depth));
#endif // TACO_DEBUG
    
    node.key = CalcMortonKey(node.anchor, max_depth, depth_bit_width);
  }
  
  return nodes;
}


#endif // TACO_HOT_
