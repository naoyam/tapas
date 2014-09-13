

template <int DIM, class FP> inline
taco::Vec<DIM, FP> &asn(taco::Vec<DIM, FP> &x, const vec<DIM, FP> &y) {
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
static inline void FMM_P2M(TacoCell &c, real_t theta) {
  if (!c.IsLeaf()) {
    taco::Map(FMM_P2M, c.subcells(), theta);
  }
  c.attr().M = 0;
  c.attr().L = 0;
  if (c.IsLeaf()) {
    taco_kernel::P2M(c);
  } else {
    taco_kernel::M2M(c);
  }
  c.attr().R /= theta;
}

static inline void FMM_L2P(TacoCell &c) {
  taco_kernel::L2L(c);
  if (c.IsLeaf()) {
#if 0    
    taco_kernel::L2P(c);
#else
    taco::Map(taco_kernel::L2P, c.particles());
#endif    
  } else {
    taco::Map(FMM_L2P, c.subcells());
  }
}


static void FMM_M2L(TacoCell &Ci, TacoCell &Cj, int mutual, int nspawn);

static inline void taco_splitCell(TacoCell &Ci, TacoCell &Cj, int mutual, int nspawn) {
  if (Cj.IsLeaf()) {
    assert(!Ci.IsLeaf());                                   //  Make sure Ci is not leaf
    //for (C_iter ci=Ci0+Ci->ICHILD; ci!=Ci0+Ci->ICHILD+Ci->NCHILD; ci++) {// Loop over Ci's children
    //traverse(ci, Cj, Xperiodic, mutual, remote);            //   Traverse a single pair of cells
    //}                                                         //
    //End loop over Ci's children
    taco::Map(FMM_M2L, taco::Product(Ci.subcells(), Cj), mutual, nspawn);
  } else if (Ci.IsLeaf()) {                               // Else if Ci is leaf
    assert(!Cj.IsLeaf());                                   //  Make sure Cj is not leaf
    //for (C_iter cj=Cj0+Cj->ICHILD; cj!=Cj0+Cj->ICHILD+Cj->NCHILD; cj++) {// Loop over Cj's children
    //traverse(Ci, cj, Xperiodic, mutual, remote);            //   Traverse a single pair of cells
    //}                                                         //
    //End loop over Cj's children
    taco::Map(FMM_M2L, taco::Product(Ci, Cj.subcells()), mutual, nspawn);
  } else if (Ci.np() + Cj.np() >= (taco::index_t)nspawn || (mutual && Ci == Cj)) {// Else if cells are still large
    //TraverseRange traverseRange(this, Ci0+Ci->ICHILD, Ci0+Ci->ICHILD+Ci->NCHILD,// Instantiate recursive functor
    //Cj0+Cj->ICHILD, Cj0+Cj->ICHILD+Cj->NCHILD, Xperiodic, mutual, remote);
    //traverseRange();                                          //
    //Traverse for range of cell pairs
    taco::Map(FMM_M2L, taco::Product(Ci.subcells(), Cj.subcells()), mutual, nspawn);
  } else if (Ci.attr().R >= Cj.attr().R) {                                // Else if Ci is larger than Cj
    //for (C_iter ci=Ci0+Ci->ICHILD; ci!=Ci0+Ci->ICHILD+Ci->NCHILD; ci++) {// Loop over Ci's children
    //traverse(ci, Cj, Xperiodic, mutual, remote);            //   Traverse a single pair of cells
    //}                                                         //  End loop over Ci's children
    taco::Map(FMM_M2L, taco::Product(Ci.subcells(), Cj), mutual, nspawn);
  } else {                                                    // Else if Cj is larger than Ci
    //for (C_iter cj=Cj0+Cj->ICHILD; cj!=Cj0+Cj->ICHILD+Cj->NCHILD; cj++) {// Loop over Cj's children
    // traverse(Ci, cj, Xperiodic, mutual, remote);            //   Traverse a single pair of cells
    //}                                                         //  End loop over Cj's children
    taco::Map(FMM_M2L, taco::Product(Ci, Cj.subcells()), mutual, nspawn);
  }
}

static inline void FMM_M2L(TacoCell &Ci, TacoCell &Cj, int mutual, int nspawn) {
  //static inline void FMM_M2L(TacoCell &Ci, TacoCell &Cj, int mutual) {
  vec3 dX = Ci.attr().X - Cj.attr().X;
  real_t R2 = norm(dX);
  vec3 Xperiodic = 0; // dummy; periodic not ported
  if (R2 > (Ci.attr().R+Cj.attr().R) * (Ci.attr().R+Cj.attr().R)) {                   // If distance is far enough
    taco_kernel::M2L(Ci, Cj, Xperiodic, mutual);                   //  M2L kernel
  } else if (Ci.IsLeaf() && Cj.IsLeaf()) {            // Else if both cells are bodies
    if (Cj.np() == 0) {                                     //  If the bodies weren't sent from remote node
      taco_kernel::M2L(Ci, Cj, Xperiodic, mutual);                 //   M2L kernel
    } else {                                                  //  Else if the bodies were sent
#if 0
      if (R2 == 0 && Ci == Cj) {                              //   If source and target are same
        taco_kernel::P2P(Ci);                                      //    P2P kernel for single cell
      } else {                                                //   Else if source and target are different
        taco_kernel::P2P(Ci, Cj, Xperiodic, mutual);               //    P2P kernel for pair of cells
      }                                                       //   End if for same source and target
#else
      taco::Map(taco_kernel::P2P, taco::Product(Ci.particles(), Cj.particles()),
                Xperiodic);
#endif
    }                                                         //  End if for bodies
  } else {                                                    // Else if cells are close but not bodies
    taco_splitCell(Ci, Cj, mutual, nspawn);             //  Split cell and call function recursively for child
  }                                                           // End if for multipole acceptance
}
