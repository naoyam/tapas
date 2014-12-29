cmake_minimum_required(VERSION 2.8)

# --------------------------------------------------------------------------------
# Find MassiveThreads
# MassiveThreads is available via https://code.google.com/p/massivethreads/
# TODO: split this part as a FindMassiveThreads module.

find_path(MYTH_INCLUDE_DIR
  NAMES "mtbb/task_group.h"
  PATHS $ENV{MYTH_ROOT}/include ${MYHT_ROOT}/include /usr/local/include)
set(MYTH_INCLUDE_DIRS ${MYTH_INCLUDE_DIR})
    
find_library(MYTH_LIBRARY
  NAMES "myth-native"
  PATHS $ENV{MYTH_ROOT}/lib ${MYTH_ROOT}/lib /usr/local/lib
  STATIC)
set(MYTH_LIBRARIES ${MYTH_LIBRARY})
message(STATUS ${MYTH_LIBRARY})

mark_as_advanced(MYTH_INCLUDE_DIRS MYTH_LIBRARIES)

if (MYTH_INCLUDE_DIR AND MYTH_LIBRARY)
  message(STATUS "Found MassiveThreads in ${MYHT_INCLUDE_DIR}")
  set(MYTH_FOUND 1)
endif()

# --------------------------------------------------------------------------------

message(STATUS "Compiler = ${CMAKE_CXX_COMPILER_ID}")

if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -ggdb3 -Wall -O2")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -funroll-loops -fcolor-diagnostics")
endif()

add_subdirectory(sample)

