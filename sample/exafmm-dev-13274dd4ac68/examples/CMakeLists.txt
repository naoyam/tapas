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
  
  set_target_properties(serial_mt
    PROPERTIES COMPILE_FLAGS "-I$ENV{HOME}/local/include -DMTHREAD")
  set_target_properties(serial_mt
    PROPERTIES LINK_FLAGS "-lmyth-native -L$ENV{HOME}/local/lib")
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

  set_target_properties(serial_simd_mt
    PROPERTIES COMPILE_FLAGS "-I$ENV{HOME}/local/include -DMTHREAD ${SIMD_FLAG}")
  set_target_properties(serial_simd_mt
    PROPERTIES LINK_FLAGS "-lmyth-native -L$ENV{HOME}/local/lib")
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

  set_target_properties(serial_tapas_mt
    PROPERTIES COMPILE_FLAGS "-I$ENV{HOME}/local/include -DMTHREAD")
  set_target_properties(serial_tapas_mt
    PROPERTIES LINK_FLAGS "-lmyth-native -L$ENV{HOME}/local/lib")
endif()

