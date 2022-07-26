# Module subdirectories

# Ports
add_fprime_subdirectory("${CMAKE_CURRENT_LIST_DIR}/DataTypes/")
add_fprime_subdirectory("${CMAKE_CURRENT_LIST_DIR}/GpioDriverPorts/")
add_fprime_subdirectory("${CMAKE_CURRENT_LIST_DIR}/SpiDriverPorts/")
add_fprime_subdirectory("${CMAKE_CURRENT_LIST_DIR}/I2cDriverPorts/")

# Components
add_fprime_subdirectory("${CMAKE_CURRENT_LIST_DIR}/BlockDriver/")
add_fprime_subdirectory("${CMAKE_CURRENT_LIST_DIR}/ByteStreamDriverModel/")
add_fprime_subdirectory("${CMAKE_CURRENT_LIST_DIR}/LinuxGpioDriver/")
add_fprime_subdirectory("${CMAKE_CURRENT_LIST_DIR}/LinuxUartDriver/")
add_fprime_subdirectory("${CMAKE_CURRENT_LIST_DIR}/LinuxSpiDriver/")
add_fprime_subdirectory("${CMAKE_CURRENT_LIST_DIR}/LinuxI2cDriver/")

# IP Socket is only supported for Linux, Darwin, VxWorks
if(${CMAKE_SYSTEM_NAME} STREQUAL "Linux" OR ${CMAKE_SYSTEM_NAME} STREQUAL "Darwin" OR ${CMAKE_SYSTEM_NAME} STREQUAL "VxWorks")
	add_fprime_subdirectory("${CMAKE_CURRENT_LIST_DIR}/Ip/")
	add_fprime_subdirectory("${CMAKE_CURRENT_LIST_DIR}/TcpClient/")
	add_fprime_subdirectory("${CMAKE_CURRENT_LIST_DIR}/TcpServer/")
	add_fprime_subdirectory("${CMAKE_CURRENT_LIST_DIR}/Udp/")
else()
    message(STATUS "Cannot use IP sockets with platform ${CMAKE_SYSTEM_NAME}. Skipping.")
endif()
