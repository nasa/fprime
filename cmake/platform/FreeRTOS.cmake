add_definitions(-DTGT_OS_TYPE_FREERTOS)

# If required, specify compiler flags here

if (NOT DEFINED FPRIME_USE_BAREMETAL_SCHEDULER)
   set(FPRIME_USE_BAREMETAL_SCHEDULER OFF)
   message(STATUS "Requiring thread library")
   FIND_PACKAGE ( Threads REQUIRED )
endif()
set(FPRIME_USE_POSIX OFF)

include_directories(SYSTEM "${FPRIME_FRAMEWORK_PATH}/Fw/Types/FreeRTOS")
