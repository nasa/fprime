add_definitions(-DTGT_OS_TYPE_RTEMS)

# Add Linux specific headers into the system
include_directories(SYSTEM "${FPRIME_CORE_DIR}/Fw/Types/Linux")

include_directories(SYSTEM
  ${RTEMS_INCLUDE_DIRS}
  ${RTEMS_BSP_INCLUDE_DIRS}
  ${RTEMS_SOURCE_DIR}/testsuites/support/include
)

add_compile_options(${RTEMS_C_FLAGS})

add_link_options(${RTEMS_LINK_FLAGS})

add_compile_definitions(
  __rtems__
  _POSIX_THREADS
)