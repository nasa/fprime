@ internal interface to send log messages to component thread
internal port loqQueue(
                        $id: FwEventIdType @< Log ID
                        timeTag: Fw.Time @< Time Tag
                        $severity: Fw.LogSeverity @< The severity argument
                        args: Fw.LogBuffer @< Buffer containing serialized log entry
                      ) \
  priority 1 \
  drop
