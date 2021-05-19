
set(CHEETAH_TEMPLATES
  "${CMAKE_CURRENT_LIST_DIR}/includes1TopologyCpp.tmpl"
  "${CMAKE_CURRENT_LIST_DIR}/includes1TopologyH.tmpl"
  "${CMAKE_CURRENT_LIST_DIR}/publicInstanceTopologyCpp.tmpl"
  "${CMAKE_CURRENT_LIST_DIR}/publicInstanceTopologyH.tmpl"
  "${CMAKE_CURRENT_LIST_DIR}/publicTopologyCpp.tmpl"
  "${CMAKE_CURRENT_LIST_DIR}/publicTopologyH.tmpl"
  "${CMAKE_CURRENT_LIST_DIR}/publicTopologyID.tmpl"
)

cheetah("${CHEETAH_TEMPLATES}")
