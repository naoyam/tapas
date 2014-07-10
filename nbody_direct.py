#!/usr/bin/env python

import sys
import itertools
import os
import re
import traceback
from optparse import OptionParser
import random
import math

from taco import *

NDIM = 2

class MyParticle(Particle):
    def __init__(self, pos, m=0.0, f=None):
        super(MyParticle, self).__init__(pos)
        self.m = m
        self.f = f
        if f is None:
            self.f = Vector([0.0] * len(pos))
        assert len(pos) == len(self.f)

    def __str__(self):
        return "particle{ pos: " + ", ".join(str(i) for i in self.pos) \
            + ", force: " + ", ".join(str(i) for i in self.f) + "}"
    pass
    
def init(np):
    particles = []
    random.seed(0)
    for i in range(np):
        pos = [random.random() for i in range(NDIM)]
        m = 1.0 / np
        p = MyParticle(pos, m)
        particles.append(p)
    return particles

def direct(p1, p2, reaction=False):
    for i, p in enumerate(p1):
        if reaction:
            j = i + 1
        else:
            j = 0
        for q in p2[j:]:
            if p is q: continue
            R2 = distance_r2(p, q)
            invR2 = 1.0 / R2
            invR = q.m * math.sqrt(invR2)
            invR3 = invR2 * invR
            for i in range(NDIM):
                f = - (p.pos[i] - q.pos[i]) * invR3
                p.f[i] -= f
                q.f[i] += f
    return

def reference_compute(np):
    particles = init(np)
    direct(particles, particles, True)
    return particles
    

def init_cell(particles, r, np):
    return Cell(particles, r, set(range(np)))

def interact(c1, c2, s):
    if c1.get_num_particles() <= s and \
       c2.get_num_particles() <= s:

        direct([c1.particles[i] for i in c1.indices],
               [c2.particles[i] for i in c2.indices],
               c1.region == c2.region)
    else:
        if c1.get_num_particles() > s:
            c1_children = c1.partition()
        else:
            c1_children = [c1]
        if c2.get_num_particles() > s:
            c2_children = c2.partition()
        else:
            c2_children = [c2]
        for i, j in product(c1_children, c2_children):
            interact(i, j, s)
    return

def taco_compute(np, s):
    particles = init(np)
    root_region =  Region(tuple([(0.0, 1.0)]*NDIM))
    root_cell = init_cell(particles, root_region, np)
    print "root cell: " + str(root_cell)
    interact(root_cell, root_cell, s)
    return particles

def diff(p1, p2):
    total_diff = 0.0
    for i, j in itertools.izip(p1, p2):
        total_diff += reduce(lambda x, y: x + y,
                             ((f - g) ** 2 for f, g in itertools.izip(i.f, j.f)))
    total_diff /= len(p1)
    return total_diff
    
def main():
    np = 100 # default number of particles
    # the first argument, if given, overrides the default value of np
    if len(sys.argv) > 1:
        np = int(sys.argv[1])


    reference_result = reference_compute(np)
    
    maximum_np_cell = int(math.ceil(np / 10.0))
    taco_result = taco_compute(np, maximum_np_cell)

    print "Reference"
    for p in reference_result: print p

    print "Taco"
    for p in taco_result: print p

    # compare results    
    print "Diff: " + str(diff(reference_result, taco_result))
        
    return
    
if __name__ == "__main__":
    main()

