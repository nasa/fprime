include(autocoder/ai-shared)

set(CHEETAH_TEMPLATES
  "${CMAKE_CURRENT_LIST_DIR}/cpp.tmpl"
  "${CMAKE_CURRENT_LIST_DIR}/hpp.tmpl"
  "${CMAKE_CURRENT_LIST_DIR}/test_helpers.tmpl"
  "${CMAKE_CURRENT_LIST_DIR}/test_main.tmpl"
)

cheetah("${CHEETAH_TEMPLATES}")
