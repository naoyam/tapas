#!/usr/bin/env python

import sys
import itertools
import math

import taco
import common

def interact(c1, c2, s):
    if c1.get_num_particles() <= s and \
       c2.get_num_particles() <= s:

        common.direct([c1.particles[i] for i in c1.indices],
                      [c2.particles[i] for i in c2.indices],
                      c1.region == c2.region)
    else:
        if c1.get_num_particles() > s:
            c1_children = c1.get_sub_cells()
        else:
            c1_children = [c1]
        if c2.get_num_particles() > s:
            c2_children = c2.get_sub_cells()
        else:
            c2_children = [c2]
        for i, j in taco.product(c1_children, c2_children):
            interact(i, j, s)
    return

def taco_compute(np):
    maximum_np_cell = int(math.ceil(np / 10.0))    
    particles = common.gen_particle_list(np)
    root_region =  taco.Region(tuple([(0.0, 1.0)]*common.NDIM))
    root_cell = taco.Cell(particles, root_region, set(range(np)))
    print "root cell: " + str(root_cell)
    interact(root_cell, root_cell, maximum_np_cell)
    return particles

def main():
    common.run_test(taco_compute, "Direct")
    return
    
if __name__ == "__main__":
    main()

