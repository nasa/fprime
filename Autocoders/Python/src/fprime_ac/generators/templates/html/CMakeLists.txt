
set(CHEETAH_TEMPLATES
  "${CMAKE_CURRENT_LIST_DIR}/HtmlChannelTablePage.tmpl"
  "${CMAKE_CURRENT_LIST_DIR}/HtmlCmdTablePage.tmpl"
  "${CMAKE_CURRENT_LIST_DIR}/HtmlDocPage.tmpl"
  "${CMAKE_CURRENT_LIST_DIR}/HtmlEventsTablePage.tmpl"
)

cheetah("${CHEETAH_TEMPLATES}")
