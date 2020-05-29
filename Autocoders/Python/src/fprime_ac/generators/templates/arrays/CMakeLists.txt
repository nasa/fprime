
set(CHEETAH_TEMPLATES
  "${CMAKE_CURRENT_LIST_DIR}/array_cpp.tmpl"
  "${CMAKE_CURRENT_LIST_DIR}/array_hpp.tmpl"
  "${CMAKE_CURRENT_LIST_DIR}/array_py.tmpl"
)

cheetah("${CHEETAH_TEMPLATES}")
