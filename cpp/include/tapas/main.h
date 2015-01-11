#ifndef TAPAS_MAIN_H_
#define TAPAS_MAIN_H_

#include "tapas/common.h"

//!\todo this code should be written in user's code
#include "single_node_morton_hot.h"

namespace tapas {

template <class BT, int POS_OFFSET>
class BodyInfo {
  public:
    typedef BT type;
    static const int pos_offset = POS_OFFSET;
};

// Generic Definition of the main class of Tapas framework
template <int DIM, class FP, class BT,
          class BT_ATTR, class CELL_ATTR,
          class PartitionAlgorithm>
class Tapas {};

} // namespace tapas


#endif // TAPAS_MAIN_H_ 
