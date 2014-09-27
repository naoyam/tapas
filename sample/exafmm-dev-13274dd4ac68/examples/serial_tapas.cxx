#include "args.h"
#include "bound_box.h"
#ifdef CILK
#include "build_tree3.h"
#else
#include "build_tree.h"
#endif
#include "dataset.h"
#include "logger.h"
#include "traversal.h"
#include "up_down_pass.h"
#include "verify.h"

#define TAPAS

#ifdef TAPAS
#include "tapas_common.h"
#include "tapas_kernel.h"
#include "serial_tapas_helper.cxx"
#endif

int main(int argc, char ** argv) {
  Args args(argc, argv);
  Bodies bodies, bodies2, bodies3, jbodies;
  BoundBox boundBox(args.nspawn);
  Bounds bounds;
  BuildTree buildTree(args.ncrit, args.nspawn);
  Cells cells, jcells;
  Dataset data;
  Traversal traversal(args.nspawn, args.images);
  UpDownPass upDownPass(args.theta, args.useRmax, args.useRopt);
  Verify verify;

#ifdef TAPAS  
Region tr;
#endif  

  num_threads(args.threads);

  const real_t cycle = 2 * M_PI;
  logger::verbose = args.verbose;
  logger::printTitle("FMM Parameters");
  args.print(logger::stringLength, P);
  bodies = data.initBodies(args.numBodies, args.distribution, 0);

#if IneJ
  for (B_iter B=bodies.begin(); B!=bodies.end(); B++) {
    B->X[0] += M_PI;
    B->X[0] *= 0.5;
  }
  jbodies = data.initBodies(args.numBodies, args.distribution, 1);
  for (B_iter B=jbodies.begin(); B!=jbodies.end(); B++) {
    B->X[0] -= M_PI;
    B->X[0] *= 0.5;
  }
#endif
  for (int t=0; t<args.repeat; t++) {
    logger::printTitle("FMM Profiling");
    logger::startTimer("Total FMM");
    logger::startPAPI();
    logger::startDAG();
    bounds = boundBox.getBounds(bodies);
#ifdef TAPAS    
    asn(tr, bounds);
    TAPAS_LOG_DEBUG() << "Bounding box: " << tr << std::endl;
#endif    
#if IneJ
    bounds = boundBox.getBounds(jbodies,bounds);
#endif
#ifndef TAPAS
    cells = buildTree.buildTree(bodies, bounds);
    upDownPass.upwardPass(cells);    
#else    
    Tapas::Cell *root = Tapas::Partition(
        bodies.data(), args.numBodies, tr, args.ncrit);
    tapas::Map(FMM_P2M, *root, args.theta);
    TAPAS_LOG_DEBUG() << "P2M done\n";
#endif

#ifndef TAPAS    
#if IneJ
    jcells = buildTree.buildTree(jbodies, bounds);
    upDownPass.upwardPass(jcells);
    traversal.dualTreeTraversal(cells, jcells, cycle, false);
#else
    traversal.dualTreeTraversal(cells, cells, cycle, args.mutual);
    jbodies = bodies;
#endif
#else
    //tapas::Map<3, real_t, Body, 0, kvec4, CellAttr, TapasCell, TapasCell, bool, int>(FMM_M2L, tapas::Product(*root, *root), args.mutual, args.nspawn);
    //tapas::Map(FMM_M2L, tapas::Product(*root, *root), args.mutual,
    //args.nspawn);
    tapas::Map(FMM_M2L, tapas::Product(*root, *root), args.mutual, args.nspawn);
    TAPAS_LOG_DEBUG() << "M2L done\n";
    jbodies = bodies;
#endif    

#ifndef TAPAS    
    upDownPass.downwardPass(cells);
#else
    tapas::Map(FMM_L2P, *root);
    TAPAS_LOG_DEBUG() << "L2P done\n";
#endif    
    
    logger::printTitle("Total runtime");
    logger::stopPAPI();
    logger::stopTimer("Total FMM");
    logger::resetTimer("Total FMM");
#if WRITE_TIME
    logger::writeTime();
#endif
    const int numTargets = 100;
    bodies3 = bodies;
    data.sampleBodies(bodies, numTargets);
    bodies2 = bodies;
    data.initTarget(bodies);
    logger::startTimer("Total Direct");
    traversal.direct(bodies, jbodies, cycle);
    traversal.normalize(bodies);
    logger::stopTimer("Total Direct");
    double potDif = verify.getDifScalar(bodies, bodies2);
    double potNrm = verify.getNrmScalar(bodies);
    double accDif = verify.getDifVector(bodies, bodies2);
    double accNrm = verify.getNrmVector(bodies);
    logger::printTitle("FMM vs. direct");
    verify.print("Rel. L2 Error (pot)",std::sqrt(potDif/potNrm));
    verify.print("Rel. L2 Error (acc)",std::sqrt(accDif/accNrm));
    buildTree.printTreeData(cells);
    traversal.printTraversalData();
    logger::printPAPI();
    logger::stopDAG();
    bodies = bodies3;
    data.initTarget(bodies);
  }
  logger::writeDAG();
  return 0;
}
