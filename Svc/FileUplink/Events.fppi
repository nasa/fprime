@ During receipt of a file, the computed checksum value did not match the stored value
event BadChecksum(
                   fileName: string size 40 @< The file name
                   computed: U32 @< The computed value
                   read: U32 @< The value read
                 ) \
  severity warning high \
  id 0 \
  format "Bad checksum value during receipt of file {}: computed 0x{x}, read 0x{x}"

@ An error occurred opening a file
event FileOpenError(
                     fileName: string size 40 @< The name of the file
                   ) \
  severity warning high \
  id 1 \
  format "Could not open file {}"

@ The File Uplink component successfully received a file
event FileReceived(
                    fileName: string size 40 @< The name of the file
                  ) \
  severity activity high \
  id 2 \
  format "Received file {}"

@ An error occurred writing to a file
event FileWriteError(
                      fileName: string size 40 @< The name of the file
                    ) \
  severity warning high \
  id 3 \
  format "Could not write to file {}" \
  throttle 5

@ The File Uplink component received a packet with a type that was invalid for the current receive mode
event InvalidReceiveMode(
                          packetType: U32 @< The type of the packet received
                          mode: U32 @< The receive mode
                        ) \
  severity warning high \
  id 4 \
  format "Packet type {} received in mode {}" \
  throttle 5

@ During receipt of a file, the File Uplink component encountered a packet with offset and size out of bounds for the current file
event PacketOutOfBounds(
                         packetIndex: U32 @< The sequence index of the packet
                         fileName: string size 40 @< The name of the file
                       ) \
  severity warning high \
  id 5 \
  format "Packet {} out of bounds for file {}" \
  throttle 5

@ The File Uplink component encountered an out-of-order packet during file receipt
event PacketOutOfOrder(
                        packetIndex: U32 @< The sequence index of the out-of-order packet
                        lastPacketIndex: U32 @< The sequence index of the last packet received before the out-of-order packet
                      ) \
  severity warning high \
  id 6 \
  format "Received packet {} after packet {}" \
  throttle 20

@ The File Uplink component encountered a duplicate packet during file receipt
event PacketDuplicate(
                        packetIndex: U32 @< The sequence index of the duplicate packet
                      ) \
  severity warning high \
  id 7 \
  format "Received a duplicate of packet {}" \
  throttle 20

@ The File Uplink component received a CANCEL packet
event UplinkCanceled \
  severity activity high \
  id 8 \
  format "Received CANCEL packet"

@ Error decoding file packet
event DecodeError(
                   status: I32 @< The sequence index of the out-of-order packet
                 ) \
  severity warning high \
  id 9 \
  format "Unable to decode file packet. Status: {}"
