#!/usr/bin/env python

import sys
import itertools
import math

import taco
import common

def interact(c1, c2, s):
    if c1.is_leaf() and c2.is_leaf():
        common.direct([c1.particles[i] for i in c1.indices],
                      [c2.particles[i] for i in c2.indices],
                      c1.region == c2.region)
    return
        

def taco_compute(np):
    maximum_np_cell = int(math.ceil(np / 10.0))    
    particles = common.gen_particle_list(np)
    root_region =  taco.Region(tuple([(0.0, 1.0)]*common.NDIM))
    root_cell = taco.Cell(particles, root_region, set(range(np)))
    print "root cell: " + str(root_cell)
    #interact(root_cell, root_cell, maximum_np_cell)

    map(interact, traverse(root_cell, root_cell), s)
    return particles

def main():
    common.run_test(taco_compute, "Direct")
    return
    
if __name__ == "__main__":
    main()

