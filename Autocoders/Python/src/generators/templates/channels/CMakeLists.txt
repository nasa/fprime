
set(CHEETAH_TEMPLATES
  "${CMAKE_CURRENT_LIST_DIR}/ChannelBody.tmpl"
  "${CMAKE_CURRENT_LIST_DIR}/ChannelHeader.tmpl"
)

cheetah("${CHEETAH_TEMPLATES}")
