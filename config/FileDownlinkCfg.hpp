/*
 * FileDownlinkCfg.hpp:
 *
 * Configuration settings for file downlink component.
 */

#ifndef SVC_FILEDOWNLINK_FILEDOWNLINKCFG_HPP_
#define SVC_FILEDOWNLINK_FILEDOWNLINKCFG_HPP_
#include <Fw/Types/BasicTypes.hpp>

namespace Svc {
    // If this is set to true, the run handler will look to
    // see if a packet is ready. If it is false, the next packet
    // will be sent as soon as the previous is complete.
    static const bool FILEDOWNLINK_PACKETS_BY_RUN = false;
    // If this is set to true, if a file is not found the command will fail.
    // If it is false, the command will still pass.
    // The false setting is to prevent a sequence from ending with
    // an error simply because one file name was wrong
    static const bool FILEDOWNLINK_COMMAND_FAIL_ON_MISSING_FILE = false;
    // Size of the internal file downlink buffer. This must now be static as
    // file down maintains its own internal buffer.
    static const U32 FILEDOWNLINK_INTERNAL_BUFFER_SIZE = FW_COM_BUFFER_MAX_SIZE-sizeof(FwPacketDescriptorType);
}

#endif /* SVC_FILEDOWNLINK_FILEDOWNLINKCFG_HPP_ */
