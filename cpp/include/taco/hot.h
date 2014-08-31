#ifndef TACO_HOT_
#define TACO_HOT_

#include "taco/stdcbug.h"

#include <cstdlib>
#include <algorithm>
#include <list>
#include <vector>
#include <unordered_set>

#include "taco/common.h"
#include "taco/vec.h"
#include "taco/basic_types.h"
#include "taco/bits.h"

namespace taco {

namespace hot {

typedef int KeyType;
typedef std::list<KeyType> KeyList;
typedef std::vector<KeyType> KeyVector;
typedef std::unordered_set<KeyType> KeySet;

template <class T>
void PrintKeys(const T &s, std::ostream &os) {
  taco::StringJoin sj;
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

template <int DIM, class FP, class PT, int OFFSET, int MAX_DEPTH>
HelperNode<DIM> *CreateInitialNodes(const PT *p, index_t np, 
                                    const Region<DIM, FP> &r);

template <int MAX_DEPTH>
KeyType MortonKeyAppendDepth(KeyType k, int depth);

template <int MAX_DEPTH>
int MortonKeyGetDepth(KeyType k);

template <int MAX_DEPTH>
KeyType MortonKeyRemoveDepth(KeyType k);

template <int MAX_DEPTH>
int MortonKeyIncrementDepth(KeyType k, int inc);

template <int DIM, int MAX_DEPTH>
bool MortonKeyIsDescendant(KeyType asc, KeyType dsc);

template <int DIM, int MAX_DEPTH>
KeyType CalcMortonKey(const Vec<DIM, int> &anchor);

template <int DIM, int MAX_DEPTH, class T>
void AppendChildren(KeyType k, T &s);

template <int DIM>
void SortNodes(HelperNode<DIM> *nodes, int n);

template <int DIM, int MAX_DEPTH>
KeyType FindFinestAncestor(KeyType x, KeyType y);

template <int DIM, int MAX_DEPTH>
void CompleteRegion(KeyType x, KeyType y, KeyVector &s);

} // namespace hot
} // namespace taco

template <int MAX_DEPTH>
taco::hot::KeyType taco::hot::MortonKeyAppendDepth(taco::hot::KeyType k, int depth) {
  int depth_bit_width = Log2<MAX_DEPTH>::x;  
  k = (k << depth_bit_width) | depth;
  return k;
}

template <int MAX_DEPTH>
int taco::hot::MortonKeyGetDepth(taco::hot::KeyType k) {
  int depth_bit_width = Log2<MAX_DEPTH>::x;  
  return k & ((1 << depth_bit_width) - 1);
}

template <int MAX_DEPTH>
taco::hot::KeyType taco::hot::MortonKeyRemoveDepth(taco::hot::KeyType k) {
  int depth_bit_width = Log2<MAX_DEPTH>::x;  
  return k >> depth_bit_width;
}

template <int MAX_DEPTH>
int taco::hot::MortonKeyIncrementDepth(taco::hot::KeyType k, int inc) {
  int depth = MortonKeyGetDepth<MAX_DEPTH>(k);
  ++depth;
  k = MortonKeyRemoveDepth<MAX_DEPTH>(k);
  return MortonKeyAppendDepth<MAX_DEPTH>(k, depth);
}

template <int DIM, int MAX_DEPTH>
bool taco::hot::MortonKeyIsDescendant(taco::hot::KeyType asc, taco::hot::KeyType dsc) {
  int depth_bit_width = Log2<MAX_DEPTH>::x;
  int depth = MortonKeyGetDepth<MAX_DEPTH>(asc);
  if (depth >= MortonKeyGetDepth<MAX_DEPTH>(dsc)) return false;
  int s = (MAX_DEPTH - depth) * DIM + depth_bit_width;
  asc >>= s;
  dsc >>= s;
  return asc == dsc;
}


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
  return MortonKeyAppendDepth<MAX_DEPTH>(k, MAX_DEPTH);
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

template <int DIM>
void taco::hot::SortNodes(taco::hot::HelperNode<DIM> *nodes, int n) {
  std::qsort(nodes, n, sizeof(HelperNode<DIM>),
             [] (const void *x, const void *y) {
               return static_cast<const HelperNode<DIM>*>(x)->key -
                   static_cast<const HelperNode<DIM>*>(y)->key;
             });
}

template <int DIM, int MAX_DEPTH>
taco::hot::KeyType taco::hot::FindFinestAncestor(taco::hot::KeyType x,
                                                 taco::hot::KeyType y) {
  int depth_bit_width = Log2<MAX_DEPTH>::x;
  int min_depth = std::min(MortonKeyGetDepth<MAX_DEPTH>(x), MortonKeyGetDepth<MAX_DEPTH>(y));
  x >>= depth_bit_width;
  y >>= depth_bit_width;
  KeyType a = ~(x ^ y);
  int common_depth = 0;
  for (; common_depth < min_depth; ++common_depth) {
    KeyType t = (a >> (MAX_DEPTH - common_depth -1) * DIM) & ((1 << DIM) - 1);
    if (t != ((1 << DIM) -1)) break;
  }
  int common_bit_len = common_depth * DIM;
  KeyType mask = ((1 << common_bit_len) - 1) << (MAX_DEPTH * DIM - common_bit_len);
  return MortonKeyAppendDepth<MAX_DEPTH>(x & mask, common_depth);
}

template <int DIM, int MAX_DEPTH, class T>
void taco::hot::AppendChildren(taco::hot::KeyType x, T &s) {
  int x_depth = MortonKeyGetDepth<MAX_DEPTH>(x);
  int depth_bit_width = Log2<MAX_DEPTH>::x;
  int c_depth = x_depth + 1;
  if (c_depth > MAX_DEPTH) return;
  x = MortonKeyIncrementDepth<MAX_DEPTH>(x, 1);
  for (int i = 0; i < (1 << DIM); ++i) {
    int child_key = (i << ((MAX_DEPTH - c_depth) * DIM + depth_bit_width));
    s.push_back(x | child_key);
    std::cout << "Adding child " << (x | child_key) << std::endl;
  }
}


template <int DIM, int MAX_DEPTH>
void taco::hot::CompleteRegion(taco::hot::KeyType x, taco::hot::KeyType y,
                               taco::hot::KeyVector &s) {
  KeyType fa = FindFinestAncestor<DIM, MAX_DEPTH>(x, y);
  KeyList w;
  AppendChildren<DIM, MAX_DEPTH>(fa, w);
  PrintKeys(w, std::cout);
  while (w.size() > 0) {
    KeyType k = w.front();
    w.pop_front();
    std::cout << "visiting " << k << std::endl;
    if ((k > x && k < y) && !MortonKeyIsDescendant<DIM, MAX_DEPTH>(k, y)) {
      s.push_back(k);
      std::cout << "Adding " << k << " to output set" << std::endl;
    } else if (MortonKeyIsDescendant<DIM, MAX_DEPTH>(k, x) ||
               MortonKeyIsDescendant<DIM, MAX_DEPTH>(k, y)) {
      std::cout << "Adding children of " << k << " to work set" << std::endl;      
      AppendChildren<DIM, MAX_DEPTH>(k, w);

    }
  }
  std::sort(std::begin(s), std::end(s));
  // TODO: sort s
}

#endif // TACO_HOT_
