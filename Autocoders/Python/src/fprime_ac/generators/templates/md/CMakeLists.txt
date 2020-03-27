
set(CHEETAH_TEMPLATES
  "${CMAKE_CURRENT_LIST_DIR}/MdChannelTablePage.tmpl"
  "${CMAKE_CURRENT_LIST_DIR}/MdCmdTablePage.tmpl"
  "${CMAKE_CURRENT_LIST_DIR}/MdDocPage.tmpl"
  "${CMAKE_CURRENT_LIST_DIR}/MdEventsTablePage.tmpl"
)

cheetah("${CHEETAH_TEMPLATES}")
