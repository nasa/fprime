@ Open error
event GP_OpenError(
                    gpio: I32 @< The device
                    error: I32 @< The error code
                    msg: string size 40 @< The error string
                  ) \
  severity warning high \
  id 0 \
  format "Error opening GPIO device {}: {} ({})"

@ GPIO configure error
event GP_ConfigError(
                      gpio: I32 @< The device
                      error: I32 @< The error code
                    ) \
  severity warning high \
  id 1 \
  format "Error configuring GPIO device {}: {}"

@ GPIO write error
event GP_WriteError(
                     gpio: I32 @< The device
                     error: I32 @< The error code
                   ) \
  severity warning high \
  id 2 \
  format "Error writing GPIO device {}: {}" \
  throttle 5

@ GPIO read error
event GP_ReadError(
                    gpio: I32 @< The device
                    error: I32 @< The error code
                  ) \
  severity warning high \
  id 3 \
  format "Error reading GPIO device {}: {}" \
  throttle 5

@ GPIO opened notification
event GP_PortOpened(
                     gpio: I32 @< The device
                   ) \
  severity activity high \
  id 4 \
  format "GPIO Device {} configured"

@ GPIO interrupt start error notification
event GP_IntStartError(
                        gpio: I32 @< The device
                      ) \
  severity warning high \
  id 5 \
  format "GPIO Device {} interrupt start error"

@ GPIO interrupt wait error notification
event GP_IntWaitError(
                       gpio: I32 @< The device
                     ) \
  severity warning high \
  id 6 \
  format "GPIO Device {} interrupt wait error"
