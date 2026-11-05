if (BUILD_TESTING)
  register_fprime_library(
    SOURCES
      "${CMAKE_CURRENT_LIST_DIR}/TestUtils.cpp"
    DEPENDS
      Svc_Ccsds_Types
      STest
  )
endif()

register_fprime_ut(
  SOURCES
    "${CMAKE_CURRENT_LIST_DIR}/main.cpp"
)
