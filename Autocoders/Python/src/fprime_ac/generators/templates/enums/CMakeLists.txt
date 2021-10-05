include(autocoder/ai-shared)

set(CHEETAH_TEMPLATES
  "${CMAKE_CURRENT_LIST_DIR}/enum_cpp.tmpl"
  "${CMAKE_CURRENT_LIST_DIR}/enum_hpp.tmpl"
)

cheetah("${CHEETAH_TEMPLATES}")
