@ Raw socket open error
event OpenError(
                device: string size 40 @< The device
                error: I32 @< The error code
              ) \
  severity warning high \
  id 0 \
  format "Error opening raw socket on {}: {}"

@ Raw socket bind error
event BindError(
                device: string size 40 @< The device
                error: I32 @< The error code
              ) \
  severity warning high \
  id 1 \
  format "Error binding raw socket on {}: {}"

@ Raw send error
event SendError(
                error: I32 @< The error code
              ) \
  severity warning high \
  id 2 \
  format "Error sending raw frame: {}" \
  throttle 10

@ Raw receive error
event RecvError(
                error: I32 @< The error code
              ) \
  severity warning high \
  id 3 \
  format "Error receiving raw frame: {}" \
  throttle 10
