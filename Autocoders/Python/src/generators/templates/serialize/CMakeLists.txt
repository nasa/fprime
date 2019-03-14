
set(CHEETAH_TEMPLATES
  "${CMAKE_CURRENT_LIST_DIR}/finishSerialCpp.tmpl"
  "${CMAKE_CURRENT_LIST_DIR}/finishSerialH.tmpl"
  "${CMAKE_CURRENT_LIST_DIR}/includes1SerialCpp.tmpl"
  "${CMAKE_CURRENT_LIST_DIR}/includes1SerialH.tmpl"
  "${CMAKE_CURRENT_LIST_DIR}/includes2SerialH.tmpl"
  "${CMAKE_CURRENT_LIST_DIR}/namespaceSerialCpp.tmpl"
  "${CMAKE_CURRENT_LIST_DIR}/namespaceSerialH.tmpl"
  "${CMAKE_CURRENT_LIST_DIR}/privateSerialH.tmpl"
  "${CMAKE_CURRENT_LIST_DIR}/protectedSerialH.tmpl"
  "${CMAKE_CURRENT_LIST_DIR}/publicSerialCpp.tmpl"
  "${CMAKE_CURRENT_LIST_DIR}/publicSerialH.tmpl"
  "${CMAKE_CURRENT_LIST_DIR}/SerialBody.tmpl"
  "${CMAKE_CURRENT_LIST_DIR}/SerialHeader.tmpl"
  "${CMAKE_CURRENT_LIST_DIR}/SerialImport.tmpl"
  "${CMAKE_CURRENT_LIST_DIR}/startSerialH.tmpl"
)

cheetah("${CHEETAH_TEMPLATES}")
