#CXX = clang++ -std=c++11 -stdlib=libc++ 
CXX = g++ -std=c++11

CXX += -O0 -g3 -Wall -fno-elide-constructors

TAPAS_DIR = ../../cpp

ref: nbody
tapas: direct_tapas

nbody: nbody.o
	$(CXX) $^ -o $@

nbody.o: nbody.cxx
	$(CXX) -DDUMP -O2 -c $^ -Wall

direct_tapas.o: direct_tapas.cc
	$(CXX) -DDUMP -O2 -c $^ -Wall -I$(TAPAS_DIR)/include

direct_tapas: direct_tapas.o
	$(CXX) $^ -o $@

clean:
	$(RM) -f *.o direct_tapas nbody
