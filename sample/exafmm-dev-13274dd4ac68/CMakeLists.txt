set(EQUATION "Laplace")
set(BASIS    "Spherical")
set(EXPANSION 4)
set(PRECISION "FP64")
set(DEVICE    "CPU")

if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -ggdb3 -Wall -O2")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -funroll-loops -fcolor-diagnostics")
endif()

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D${PRECISION}")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D${BASIS}")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DEXPANSION=${EXPANSION}")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DASSERT")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DCOUNT")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DTAPAS_LOG_LEVEL=0 -DTAPAS_DEBUG=0")

# ExaFMM's include directory
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/include")

# Tapas' C++ include directory
include_directories("${PROJECT_SOURCE_DIR}/cpp/include")

add_subdirectory(examples)


