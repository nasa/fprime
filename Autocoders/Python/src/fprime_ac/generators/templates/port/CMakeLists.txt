
set(CHEETAH_TEMPLATES
  "${CMAKE_CURRENT_LIST_DIR}/finishPortCpp.tmpl"
  "${CMAKE_CURRENT_LIST_DIR}/finishPortH.tmpl"
  "${CMAKE_CURRENT_LIST_DIR}/includes1PortCpp.tmpl"
  "${CMAKE_CURRENT_LIST_DIR}/includes1PortH.tmpl"
  "${CMAKE_CURRENT_LIST_DIR}/includes2PortH.tmpl"
  "${CMAKE_CURRENT_LIST_DIR}/namespacePortCpp.tmpl"
  "${CMAKE_CURRENT_LIST_DIR}/namespacePortH.tmpl"
  "${CMAKE_CURRENT_LIST_DIR}/privatePortCpp.tmpl"
  "${CMAKE_CURRENT_LIST_DIR}/privatePortH.tmpl"
  "${CMAKE_CURRENT_LIST_DIR}/protectedPortH.tmpl"
  "${CMAKE_CURRENT_LIST_DIR}/publicPortCpp.tmpl"
  "${CMAKE_CURRENT_LIST_DIR}/publicPortH.tmpl"
  "${CMAKE_CURRENT_LIST_DIR}/startPortH.tmpl"
)

cheetah("${CHEETAH_TEMPLATES}")
