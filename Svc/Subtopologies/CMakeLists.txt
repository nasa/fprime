add_fprime_subdirectory("${CMAKE_CURRENT_LIST_DIR}/CdhCore/")
add_fprime_subdirectory("${CMAKE_CURRENT_LIST_DIR}/ComCcsds/")
add_fprime_subdirectory("${CMAKE_CURRENT_LIST_DIR}/ComFprime/")
add_fprime_subdirectory("${CMAKE_CURRENT_LIST_DIR}/FileHandling/")
add_fprime_subdirectory("${CMAKE_CURRENT_LIST_DIR}/DataProducts/")
add_fprime_subdirectory("${CMAKE_CURRENT_LIST_DIR}/ComLoggerTee/")

register_fprime_library(
    SOURCES 
        "${CMAKE_CURRENT_LIST_DIR}/empty.cpp"
    DEPENDS
        Svc_Subtopologies_CdhCore
        Svc_Subtopologies_ComCcsds
        Svc_Subtopologies_ComFprime
        Svc_Subtopologies_FileHandling
        Svc_Subtopologies_DataProducts
)