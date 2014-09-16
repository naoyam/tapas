#ifndef TAPAS_HOT_
#define TAPAS_HOT_

#include "tapas/stdcbug.h"

#include <cstdlib>
#include <algorithm>
#include <list>
#include <vector>
#include <unordered_set>

#include "tapas/common.h"
#include "tapas/vec.h"
#include "tapas/basic_types.h"
#include "tapas/bits.h"

namespace tapas {

namespace hot {

typedef int KeyType;
typedef std::list<KeyType> KeyList;
typedef std::vector<KeyType> KeyVector;
typedef std::unordered_set<KeyType> KeySet;

template <class T>
void PrintKeys(const T &s, std::ostream &os) {
  tapas::StringJoin sj;
  for (auto k: s) {
    sj << k;
  }
  os << "Key set: " << sj << std::endl;
}

template <int DIM>
struct HelperNode {
  KeyType key; // Morton key
  Vec<DIM, int> anchor;
  index_t p_index;
  index_t np;
};

template <int DIM, class FP, class PT, int OFFSET>
HelperNode<DIM> *CreateInitialNodes(const PT *p, index_t np, 
                                    const Region<DIM, FP> &r,
                                    const int max_depth);

KeyType MortonKeyAppendDepth(KeyType k, int depth, const int depth_bit_width);

int MortonKeyGetDepth(KeyType k, const int depth_bit_width);

KeyType MortonKeyRemoveDepth(KeyType k, const int depth_bit_width);

int MortonKeyIncrementDepth(KeyType k, int inc, const int depth_bit_width);

template <int DIM>
bool MortonKeyIsDescendant(KeyType asc, KeyType dsc, const int max_depth,
                           const int depth_bit_width);

template <int DIM>
KeyType CalcMortonKey(const Vec<DIM, int> &anchor, const int max_depth,
                      const int depth_bit_width);

template <int DIM, class T>
void AppendChildren(KeyType k, T &s, const int max_depth, const int depth_bit_width);

template <int DIM>
void SortNodes(HelperNode<DIM> *nodes, int n);

template <int DIM>
KeyType FindFinestAncestor(KeyType x, KeyType y, const int max_depth,
                           const int depth_bit_width);

template <int DIM>
void CompleteRegion(KeyType x, KeyType y, KeyVector &s, const int max_depth);

} // namespace hot
} // namespace tapas

inline
tapas::hot::KeyType tapas::hot::MortonKeyAppendDepth(tapas::hot::KeyType k, int depth,
                                                   const int depth_bit_width) {
  k = (k << depth_bit_width) | depth;
  return k;
}

inline
int tapas::hot::MortonKeyGetDepth(tapas::hot::KeyType k,
                                 const int depth_bit_width) {
  return k & ((1 << depth_bit_width) - 1);
}

inline
tapas::hot::KeyType tapas::hot::MortonKeyRemoveDepth(tapas::hot::KeyType k,
                                                   const int depth_bit_width) {
  return k >> depth_bit_width;
}

inline
int tapas::hot::MortonKeyIncrementDepth(tapas::hot::KeyType k, int inc,
                                       const int depth_bit_width) {
  int depth = MortonKeyGetDepth(k, depth_bit_width);
  ++depth;
  k = MortonKeyRemoveDepth(k, depth_bit_width);
  return MortonKeyAppendDepth(k, depth, depth_bit_width);
}

template <int DIM>
bool tapas::hot::MortonKeyIsDescendant(tapas::hot::KeyType asc, tapas::hot::KeyType dsc,
                                      const int max_depth, const int depth_bit_width) {
  int depth = MortonKeyGetDepth(asc, depth_bit_width);
  if (depth >= MortonKeyGetDepth(dsc, depth_bit_width)) return false;
  int s = (max_depth - depth) * DIM + depth_bit_width;
  asc >>= s;
  dsc >>= s;
  return asc == dsc;
}


template <int DIM>
tapas::hot::KeyType tapas::hot::CalcMortonKey(const tapas::Vec<DIM, int> &anchor,
                                            const int max_depth,
                                            const int depth_bit_width) {
  KeyType k = 0;
  int mask = 1 << (max_depth - 1);
  for (int i = 0; i < max_depth; ++i) {
    for (int d = 0; d < DIM; ++d) {
      k = (k << 1) | ((anchor[d] & mask) >> (max_depth - i - 1));
    }
    mask >>= 1;
  }
  return MortonKeyAppendDepth(k, max_depth, depth_bit_width);
}

template <int DIM, class FP, class PT, int OFFSET>
tapas::hot::HelperNode<DIM> *tapas::hot::CreateInitialNodes(
    const PT *p, index_t np,
    const Region<DIM, FP> &r,
    const int max_depth) {

  HelperNode<DIM> *nodes = new HelperNode<DIM>[np];
  FP num_cell = 1 << max_depth;
  Vec<DIM, FP> pitch;
  const int depth_bit_width = CalcMinBitLen(max_depth);
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
#ifdef TAPAS_DEBUG
    assert(node.anchor >= 0);
    assert(node.anchor < (1 << max_depth));
#endif // TAPAS_DEBUG
    
    node.key = CalcMortonKey<DIM>(node.anchor, max_depth, depth_bit_width);
  }
  
  return nodes;
}

template <int DIM>
void tapas::hot::SortNodes(tapas::hot::HelperNode<DIM> *nodes, int n) {
  std::qsort(nodes, n, sizeof(HelperNode<DIM>),
             [] (const void *x, const void *y) {
               return static_cast<const HelperNode<DIM>*>(x)->key -
                   static_cast<const HelperNode<DIM>*>(y)->key;
             });
}

template <int DIM>
tapas::hot::KeyType tapas::hot::FindFinestAncestor(tapas::hot::KeyType x,
                                                 tapas::hot::KeyType y,
                                                 const int max_depth,
                                                 const int depth_bit_width) {
  int min_depth = std::min(MortonKeyGetDepth(x, depth_bit_width),
                           MortonKeyGetDepth(y, depth_bit_width));
  x >>= depth_bit_width;
  y >>= depth_bit_width;
  KeyType a = ~(x ^ y);
  int common_depth = 0;
  for (; common_depth < min_depth; ++common_depth) {
    KeyType t = (a >> (max_depth - common_depth -1) * DIM) & ((1 << DIM) - 1);
    if (t != ((1 << DIM) -1)) break;
  }
  int common_bit_len = common_depth * DIM;
  KeyType mask = ((1 << common_bit_len) - 1) << (max_depth * DIM - common_bit_len);
  return MortonKeyAppendDepth(x & mask, common_depth, depth_bit_width);
}

template <int DIM, class T>
void tapas::hot::AppendChildren(tapas::hot::KeyType x, T &s,
                               const int max_depth, const int depth_bit_width) {
  int x_depth = MortonKeyGetDepth(x, depth_bit_width);
  int c_depth = x_depth + 1;
  if (c_depth > max_depth) return;
  x = MortonKeyIncrementDepth(x, 1, depth_bit_width);
  for (int i = 0; i < (1 << DIM); ++i) {
    int child_key = (i << ((max_depth - c_depth) * DIM + depth_bit_width));
    s.push_back(x | child_key);
    std::cout << "Adding child " << (x | child_key) << std::endl;
  }
}


template <int DIM>
void tapas::hot::CompleteRegion(tapas::hot::KeyType x, tapas::hot::KeyType y,
                               tapas::hot::KeyVector &s,
                               const int max_depth) {
  int depth_bit_width = CalcMinBitLen(max_depth);
  KeyType fa = FindFinestAncestor<DIM>(x, y, max_depth, depth_bit_width);
  KeyList w;
  AppendChildren<DIM>(fa, w, max_depth, depth_bit_width);
  PrintKeys(w, std::cout);
  while (w.size() > 0) {
    KeyType k = w.front();
    w.pop_front();
    std::cout << "visiting " << k << std::endl;
    if ((k > x && k < y) && !MortonKeyIsDescendant<DIM>(k, y, max_depth,
                                                        depth_bit_width)) {
      s.push_back(k);
      std::cout << "Adding " << k << " to output set" << std::endl;
    } else if (MortonKeyIsDescendant<DIM>(k, x, max_depth, depth_bit_width) ||
               MortonKeyIsDescendant<DIM>(k, y, max_depth, depth_bit_width)) {
      std::cout << "Adding children of " << k << " to work set" << std::endl;      
      AppendChildren<DIM>(k, w, max_depth, depth_bit_width);

    }
  }
  std::sort(std::begin(s), std::end(s));
}

#endif // TAPAS_HOT_
