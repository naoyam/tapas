

template <int DIM, class FP> inline
tapas::Vec<DIM, FP> &asn(tapas::Vec<DIM, FP> &x, const vec<DIM, FP> &y) {
  for (int i = 0; i < DIM; ++i) {
    x[i] = y[i];
  }
  return x;
}

static Region &asn(Region &x, const Bounds &y) {
  asn(x.min(), y.Xmin);
  asn(x.max(), y.Xmax);
  return x;
}

// UpDownPass::upwardPass
static inline void FMM_P2M(Tapas::Cell &c, real_t theta) {
  if (!c.IsLeaf()) {
    tapas::Map(FMM_P2M, c.subcells(), theta);
  }
  c.attr().M = 0;
  c.attr().L = 0;
  if (c.IsLeaf()) {
    tapas_kernel::P2M(c);
  } else {
    tapas_kernel::M2M(c);
  }
  c.attr().R /= theta;
}

static inline void FMM_L2P(Tapas::Cell &c) {
  tapas_kernel::L2L(c);
  if (c.IsLeaf()) {
#if 0    
    tapas_kernel::L2P(c);
#else
    tapas::Map(tapas_kernel::L2P, c.bodies());
#endif    
  } else {
    tapas::Map(FMM_L2P, c.subcells());
  }
}


static void FMM_M2L(Tapas::Cell &Ci, Tapas::Cell &Cj, int mutual, int nspawn);

static inline void tapas_splitCell(Tapas::Cell &Ci, Tapas::Cell &Cj, int mutual, int nspawn) {
  if (Cj.IsLeaf()) {
    assert(!Ci.IsLeaf());                                   //  Make sure Ci is not leaf
    //for (C_iter ci=Ci0+Ci->ICHILD; ci!=Ci0+Ci->ICHILD+Ci->NCHILD; ci++) {// Loop over Ci's children
    //traverse(ci, Cj, Xperiodic, mutual, remote);            //   Traverse a single pair of cells
    //}                                                         //
    //End loop over Ci's children
    tapas::Map(FMM_M2L, tapas::Product(Ci.subcells(), Cj), mutual, nspawn);
  } else if (Ci.IsLeaf()) {                               // Else if Ci is leaf
    assert(!Cj.IsLeaf());                                   //  Make sure Cj is not leaf
    //for (C_iter cj=Cj0+Cj->ICHILD; cj!=Cj0+Cj->ICHILD+Cj->NCHILD; cj++) {// Loop over Cj's children
    //traverse(Ci, cj, Xperiodic, mutual, remote);            //   Traverse a single pair of cells
    //}                                                         //
    //End loop over Cj's children
    tapas::Map(FMM_M2L, tapas::Product(Ci, Cj.subcells()), mutual, nspawn);
  } else if (Ci.nb() + Cj.nb() >= (tapas::index_t)nspawn || (mutual && Ci == Cj)) {// Else if cells are still large
    //TraverseRange traverseRange(this, Ci0+Ci->ICHILD, Ci0+Ci->ICHILD+Ci->NCHILD,// Instantiate recursive functor
    //Cj0+Cj->ICHILD, Cj0+Cj->ICHILD+Cj->NCHILD, Xperiodic, mutual, remote);
    //traverseRange();                                          //
    //Traverse for range of cell pairs
    tapas::Map(FMM_M2L, tapas::Product(Ci.subcells(), Cj.subcells()), mutual, nspawn);
  } else if (Ci.attr().R >= Cj.attr().R) {                                // Else if Ci is larger than Cj
    //for (C_iter ci=Ci0+Ci->ICHILD; ci!=Ci0+Ci->ICHILD+Ci->NCHILD; ci++) {// Loop over Ci's children
    //traverse(ci, Cj, Xperiodic, mutual, remote);            //   Traverse a single pair of cells
    //}                                                         //  End loop over Ci's children
    tapas::Map(FMM_M2L, tapas::Product(Ci.subcells(), Cj), mutual, nspawn);
  } else {                                                    // Else if Cj is larger than Ci
    //for (C_iter cj=Cj0+Cj->ICHILD; cj!=Cj0+Cj->ICHILD+Cj->NCHILD; cj++) {// Loop over Cj's children
    // traverse(Ci, cj, Xperiodic, mutual, remote);            //   Traverse a single pair of cells
    //}                                                         //  End loop over Cj's children
    tapas::Map(FMM_M2L, tapas::Product(Ci, Cj.subcells()), mutual, nspawn);
  }
}

static inline void FMM_M2L(Tapas::Cell &Ci, Tapas::Cell &Cj, int mutual, int nspawn) {
  //static inline void FMM_M2L(TapasCell &Ci, TapasCell &Cj, int mutual) {
  vec3 dX = Ci.attr().X - Cj.attr().X;
  real_t R2 = norm(dX);
  vec3 Xperiodic = 0; // dummy; periodic not ported
  if (R2 > (Ci.attr().R+Cj.attr().R) * (Ci.attr().R+Cj.attr().R)) {                   // If distance is far enough
    tapas_kernel::M2L(Ci, Cj, Xperiodic, mutual);                   //  M2L kernel
  } else if (Ci.IsLeaf() && Cj.IsLeaf()) {            // Else if both cells are bodies
    if (Cj.nb() == 0) {                                     //  If the bodies weren't sent from remote node
      tapas_kernel::M2L(Ci, Cj, Xperiodic, mutual);                 //   M2L kernel
    } else {                                                  //  Else if the bodies were sent
#if 0
      if (R2 == 0 && Ci == Cj) {                              //   If source and target are same
        tapas_kernel::P2P(Ci);                                      //    P2P kernel for single cell
      } else {                                                //   Else if source and target are different
        tapas_kernel::P2P(Ci, Cj, Xperiodic, mutual);               //    P2P kernel for pair of cells
      }                                                       //   End if for same source and target
#else
      tapas::Map(tapas_kernel::P2P, tapas::Product(Ci.bodies(), Cj.bodies()),
                Xperiodic);
#endif
    }                                                         //  End if for bodies
  } else {                                                    // Else if cells are close but not bodies
    tapas_splitCell(Ci, Cj, mutual, nspawn);             //  Split cell and call function recursively for child
  }                                                           // End if for multipole acceptance
}
