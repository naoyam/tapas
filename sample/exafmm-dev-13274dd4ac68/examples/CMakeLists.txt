set(SIMD_FLAG "-DUSE_SIMD")
# Find the appropriate compiler's SIMD flag
if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
  set(SIMD_FLAG "${SIMD_FLAG} -mavx")
  # using Clang
elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
  set(SIMD_FLAG "${SIMD_FLAG} -mavx")
  # using GCC
elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Intel")
  set(SIMD_FLAG "${SIMD_FLAG} -xHOST")
  # using Intel C++
elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
  # using Visual Studio C++
endif()

#------------------------------------------------------------------
# Original ExaFMM, w/o multithreading
#------------------------------------------------------------------
add_executable(serial
  serial.cxx
  ../kernels/${EXAFMM_EQUATION}${EXAFMM_BASIS}${EXAFMM_DEVICE}.cxx
  ../kernels/${EXAFMM_EQUATION}P2P${EXAFMM_DEVICE}.cxx
  )

set_target_properties(serial
  PROPERTIES COMPILE_FLAGS "-DTO_SERIAL")

if (EXAFMM_ENABLE_MT AND NOT MYTH_FOUND)
  message(FATAL_ERROR " You need MassiveThreads to build mutlithreaded binaries. Try using MYTH_ROOT or disable EXAFMM_ENABLE_MT")
endif()

#------------------------------------------------------------------
# Original ExaFMM with multithreading (w/o SIMD)
#------------------------------------------------------------------
if (EXAFMM_ENABLE_MT)
  add_executable(serial_mt
    serial.cxx
    ../kernels/${EXAFMM_EQUATION}${EXAFMM_BASIS}${EXAFMM_DEVICE}.cxx
    ../kernels/${EXAFMM_EQUATION}P2P${EXAFMM_DEVICE}.cxx
    )

  set_property(TARGET serial_mt APPEND PROPERTY INCLUDE_DIRECTORIES ${MYTH_INCLUDE_DIRS})
  set_property(TARGET serial_mt PROPERTY COMPILE_FLAGS "-DMTHREAD")
  target_link_libraries(serial_mt ${MYTH_LIBRARIES})
endif()

#------------------------------------------------------------------
# Original ExaFMM with multithreading (w/ SIMD)
#------------------------------------------------------------------
if (EXAFMM_ENABLE_MT)
  add_executable(serial_simd_mt
    serial.cxx
    ../kernels/${EXAFMM_EQUATION}${EXAFMM_BASIS}${EXAFMM_DEVICE}.cxx
    ../kernels/${EXAFMM_EQUATION}P2P${EXAFMM_DEVICE}.cxx
    )

  set_property(TARGET serial_simd_mt APPEND PROPERTY INCLUDE_DIRECTORIES ${MYTH_INCLUDE_DIRS})
  set_property(TARGET serial_simd_mt PROPERTY COMPILE_FLAGS "-DMTHREAD ${SIMD_FLAG}")
  target_link_libraries(serial_simd_mt ${MYTH_LIBRARIES})
endif()

#------------------------------------------------------------------
# Tapas implementation of ExaFMM w/o multithreading
#------------------------------------------------------------------
add_executable(serial_tapas
  serial_tapas.cxx
  ${EXAFMM_EQUATION}${EXAFMM_BASIS}${EXAFMM_DEVICE}_tapas.cxx
  ${EXAFMM_EQUATION}P2P${EXAFMM_DEVICE}_tapas.cxx
  ../kernels/${EXAFMM_EQUATION}${EXAFMM_BASIS}${EXAFMM_DEVICE}.cxx
  ../kernels/${EXAFMM_EQUATION}P2P${EXAFMM_DEVICE}.cxx
  )

set_target_properties(serial_tapas
  PROPERTIES COMPILE_FLAGS "-DTO_SERIAL")

#------------------------------------------------------------------
# Tapas implementation of ExaFMM with multithreading
#------------------------------------------------------------------
# Tapas implementation of ExaFMM with multithreading
if (EXAFMM_ENABLE_MT)
  add_executable(serial_tapas_mt
    serial_tapas.cxx
    ${EXAFMM_EQUATION}${EXAFMM_BASIS}${EXAFMM_DEVICE}_tapas.cxx
    ${EXAFMM_EQUATION}P2P${EXAFMM_DEVICE}_tapas.cxx
    ../kernels/${EXAFMM_EQUATION}${EXAFMM_BASIS}${EXAFMM_DEVICE}.cxx
    ../kernels/${EXAFMM_EQUATION}P2P${EXAFMM_DEVICE}.cxx
    )

  set_property(TARGET serial_tapas_mt APPEND PROPERTY INCLUDE_DIRECTORIES ${MYTH_INCLUDE_DIRS})
  set_property(TARGET serial_tapas_mt PROPERTY COMPILE_FLAGS "-DMTHREAD ${SIMD_FLAG}")
  target_link_libraries(serial_tapas_mt ${MYTH_LIBRARIES})
endif()

