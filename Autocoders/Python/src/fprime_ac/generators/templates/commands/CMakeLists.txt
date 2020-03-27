
set(CHEETAH_TEMPLATES
  "${CMAKE_CURRENT_LIST_DIR}/CommandBody.tmpl"
  "${CMAKE_CURRENT_LIST_DIR}/CommandHeader.tmpl"
)

cheetah("${CHEETAH_TEMPLATES}")
