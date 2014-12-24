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
  ../kernels/${EQUATION}${BASIS}${DEVICE}.cxx
  ../kernels/${EQUATION}P2P${DEVICE}.cxx
  )

set_target_properties(serial
  PROPERTIES COMPILE_FLAGS "-DTO_SERIAL")

#------------------------------------------------------------------
# Original ExaFMM with multithreading (w/o SIMD)
#------------------------------------------------------------------
add_executable(serial_mt
  serial.cxx
  ../kernels/${EQUATION}${BASIS}${DEVICE}.cxx
  ../kernels/${EQUATION}P2P${DEVICE}.cxx
  )

set_target_properties(serial_mt
  PROPERTIES COMPILE_FLAGS "-I$ENV{HOME}/local/include -DMTHREAD")
set_target_properties(serial_mt
  PROPERTIES LINK_FLAGS "-lmyth-native -L$ENV{HOME}/local/lib")

#------------------------------------------------------------------
# Original ExaFMM with multithreading (w/ SIMD)
#------------------------------------------------------------------
add_executable(serial_simd_mt
  serial.cxx
  ../kernels/${EQUATION}${BASIS}${DEVICE}.cxx
  ../kernels/${EQUATION}P2P${DEVICE}.cxx
  )

set_target_properties(serial_simd_mt
  PROPERTIES COMPILE_FLAGS "-I$ENV{HOME}/local/include -DMTHREAD ${SIMD_FLAG}")
set_target_properties(serial_simd_mt
  PROPERTIES LINK_FLAGS "-lmyth-native -L$ENV{HOME}/local/lib")

#------------------------------------------------------------------
# Tapas implementation of ExaFMM w/o multithreading
#------------------------------------------------------------------
add_executable(serial_tapas
  serial_tapas.cxx
  ${EQUATION}${BASIS}${DEVICE}_tapas.cxx
  ${EQUATION}P2P${DEVICE}_tapas.cxx
  ../kernels/${EQUATION}${BASIS}${DEVICE}.cxx
  ../kernels/${EQUATION}P2P${DEVICE}.cxx
  )

set_target_properties(serial_tapas
  PROPERTIES COMPILE_FLAGS "-DTO_SERIAL")

# Tapas implementation of ExaFMM with multithreading
add_executable(serial_tapas_mt
  serial_tapas.cxx
  ${EQUATION}${BASIS}${DEVICE}_tapas.cxx
  ${EQUATION}P2P${DEVICE}_tapas.cxx
  ../kernels/${EQUATION}${BASIS}${DEVICE}.cxx
  ../kernels/${EQUATION}P2P${DEVICE}.cxx
  )

set_target_properties(serial_tapas_mt
  PROPERTIES COMPILE_FLAGS "-I$ENV{HOME}/local/include -DMTHREAD")
set_target_properties(serial_tapas_mt
  PROPERTIES LINK_FLAGS "-lmyth-native -L$ENV{HOME}/local/lib")

