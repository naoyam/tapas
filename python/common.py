import random
import sys
import itertools
import math
import time
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
        return "particle { pos: " + ", ".join(str(i) for i in self.pos) \
            + ", mass: " + str(self.m) \
            + ", force: " + ", ".join(str(i) for i in self.f) + "}"
    pass
    
def gen_particle_list(np):
    particles = []
    random.seed(0)
    for i in range(np):
        pos = [random.random() for i in range(NDIM)]
        m = 1.0 / np
        p = MyParticle(pos, m)
        particles.append(p)
    return particles

def compute_force(pos1, pos2, m1, m2):
    R2 = pos1.distance_r2(pos2)
    if R2 == 0: return 0.0
    invR2 = 1.0 / R2
    invR = math.sqrt(invR2)
    invR3 = invR2 * invR * m1 * m2
    f = Vector()
    for i in range(NDIM):
        f.append((pos1[i] - pos2[i]) * invR3)
    return f

def direct(p1, p2, reaction=False):
    for i, p in enumerate(p1):
        if reaction:
            j = i + 1
        else:
            j = 0
        for q in p2[j:]:
            if p is q: continue
            f = compute_force(p.pos, q.pos, p.m, q.m)
            p.f = p.f.add(f)
            q.f = q.f.add(f.mult(-1))
    return

def reference_compute(np):
    particles = gen_particle_list(np)
    direct(particles, particles, True)
    return particles

def diff(p1, p2):
    total_diff = 0.0
    for i, j in itertools.izip(p1, p2):
        total_diff += reduce(lambda x, y: x + y,
                             ((f - g) ** 2 for f, g in itertools.izip(i.f, j.f)))
    total_diff /= len(p1)
    return total_diff
    
def run_test(test_method, descriptor_str):
    np = 100 # default number of particles
    # the first argument, if given, overrides the default value of np
    if len(sys.argv) > 1:
        np = int(sys.argv[1])

    s = time.clock()
    reference_result = reference_compute(np)
    ref_elapsed = time.clock() - s

    s = time.clock()
    test_result = test_method(np)
    test_elapsed = time.clock() - s

    if np <= 10:
        print "Reference (Direct)"
        for p in reference_result: print p

        print descriptor_str
        for p in test_result: print p

    # compare results    
    print "Diff: " + str(diff(reference_result, test_result))

    print "Reference elapsed time: %f sec" % ref_elapsed
    print "%s elapsed time: %f sec" % (descriptor_str, test_elapsed)
        
    return
