set(EXAFMM_EQUATION "Laplace"   CACHE STRING "Equasion to use")
set(EXAFMM_BASIS    "Spherical" CACHE STRING "Expansion basis (Cartesian or Spherical)")
set(EXAFMM_EXPANSION 4          CACHE STRING "Length of expansion")
set(EXAFMM_FP64      True       CACHE BOOL   "Use double")
set(EXAFMM_DEVICE    "CPU")

if (EXAFMM_FP64)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DFP64")
endif()
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D${EXAFMM_BASIS}")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DEXPANSION=${EXAFMM_EXPANSION}")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DASSERT")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DCOUNT")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DTAPAS_LOG_LEVEL=0 -DTAPAS_DEBUG=0")

# ExaFMM's include directory
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/include")

# Tapas' C++ include directory
include_directories("${PROJECT_SOURCE_DIR}/cpp/include")

add_subdirectory(examples)


