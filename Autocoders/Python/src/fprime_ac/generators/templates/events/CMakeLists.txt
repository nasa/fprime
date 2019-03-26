
set(CHEETAH_TEMPLATES
  "${CMAKE_CURRENT_LIST_DIR}/EventBody.tmpl"
  "${CMAKE_CURRENT_LIST_DIR}/EventHeader.tmpl"
)

cheetah("${CHEETAH_TEMPLATES}")
