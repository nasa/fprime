# Module subdirectories

# Ports
add_fprime_subdirectory("${CMAKE_CURRENT_LIST_DIR}/Ports/")

# Components
add_fprime_subdirectory("${CMAKE_CURRENT_LIST_DIR}/BlockDriver/")
add_fprime_subdirectory("${CMAKE_CURRENT_LIST_DIR}/ByteStreamDriverModel/")
add_fprime_subdirectory("${CMAKE_CURRENT_LIST_DIR}/LinuxGpioDriver/")
add_fprime_subdirectory("${CMAKE_CURRENT_LIST_DIR}/LinuxUartDriver/")
add_fprime_subdirectory("${CMAKE_CURRENT_LIST_DIR}/LinuxSpiDriver/")
add_fprime_subdirectory("${CMAKE_CURRENT_LIST_DIR}/LinuxI2cDriver/")
add_fprime_subdirectory("${CMAKE_CURRENT_LIST_DIR}/StreamCrossover/")

add_fprime_subdirectory("${CMAKE_CURRENT_LIST_DIR}/Ip/")
add_fprime_subdirectory("${CMAKE_CURRENT_LIST_DIR}/TcpClient/")
add_fprime_subdirectory("${CMAKE_CURRENT_LIST_DIR}/TcpServer/")
add_fprime_subdirectory("${CMAKE_CURRENT_LIST_DIR}/Udp/")
