module Svc {

  @ Sends port calls over UDP
  active component UdpSender {

    @ Input port to get called periodically for telemetry
    sync input port Sched: Svc.Sched

    @ Input ports from other components
    async input port PortsIn: [10] serial

    @ Connection opened
    event US_PortOpened(
                         server: string size 80
                         $port: U32
                       ) \
      severity activity high \
      id 0 \
      format "UdpSender port opened to server {}, port {}"

    @ UDP port socket error
    event US_SocketError(
                          error: string size 80
                        ) \
      severity warning high \
      id 1 \
      format "UdpSender socket error: {}"

    @ UDP send error
    event US_SendError(
                        error: string size 80
                      ) \
      severity warning high \
      id 2 \
      format "UDPSender send error: {}" \
      throttle 5

    @ Number of packets sent
    telemetry US_PacketsSent: U32 id 0

    @ Number of bytes sent
    telemetry US_BytesSent: U32 id 1

    # added necessary ports not in the original xml in order to compile
    @ Event port for emitting events
    event port Log

    @ Event port for emitting text events
    text event port LogText

    @ A port for getting the time
    time get port Time

    @ A port for emitting telemetry
    telemetry port Tlm 
  }

}
