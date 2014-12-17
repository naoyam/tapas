# Original ExaFMM, w/o multithreading
add_executable(serial
  serial.cxx
  ../kernels/${EQUATION}${BASIS}${DEVICE}.cxx
  ../kernels/${EQUATION}P2P${DEVICE}.cxx
  )

# Original ExaFMM with multithreading
add_executable(serial_mt
  serial.cxx
  ../kernels/${EQUATION}${BASIS}${DEVICE}.cxx
  ../kernels/${EQUATION}P2P${DEVICE}.cxx
  )

set_target_properties(serial_mt
  PROPERTIES COMPILE_FLAGS "-DMTHREAD -I$ENV{HOME}/local/include")
set_target_properties(serial_mt
  PROPERTIES LINK_FLAGS "-lmyth-native -L$ENV{HOME}/local/lib")

# Tapas implementation of ExaFMM w/o multithreading
add_executable(serial_tapas
  serial_tapas.cxx
  ${EQUATION}${BASIS}${DEVICE}_tapas.cxx
  ${EQUATION}P2P${DEVICE}_tapas.cxx
  ../kernels/${EQUATION}${BASIS}${DEVICE}.cxx
  ../kernels/${EQUATION}P2P${DEVICE}.cxx
  )

# Tapas implementation of ExaFMM with multithreading
add_executable(serial_tapas_mt
  serial_tapas.cxx
  ${EQUATION}${BASIS}${DEVICE}_tapas.cxx
  ${EQUATION}P2P${DEVICE}_tapas.cxx
  ../kernels/${EQUATION}${BASIS}${DEVICE}.cxx
  ../kernels/${EQUATION}P2P${DEVICE}.cxx
  )

set_target_properties(serial_tapas_mt
  PROPERTIES COMPILE_FLAGS "-DMTHREAD -I$ENV{HOME}/local/include")
set_target_properties(serial_tapas_mt
  PROPERTIES LINK_FLAGS "-lmyth-native -L$ENV{HOME}/local/lib")

