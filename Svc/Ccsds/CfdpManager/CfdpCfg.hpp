// ======================================================================
// \title  CfdpCfg.hpp
// \author campuzan
// \brief  F Prime CFDP configuration constants
// ======================================================================

namespace Svc {
namespace Ccsds {

/**
 *  @brief Number of channels
 *
 *  @par Description:
 *       The number of channels in the engine. Changing this
 *       value changes the configuration table for the application.
 *       This must match CfdpManagerNumBufferPorts defined in CfdpCfg.fpp
 *
 *  @par Limits:
 *       Must be less <= 200. Obviously it will be smaller than that.
 */
#define CF_NUM_CHANNELS (2)

/**
 *  @brief RX chunks per transaction (per channel)
 *
 *  @par Description:
 *       Number of chunks per transaction per channel (RX).
 *
 * CHUNKS -
 * A chunk is a representation of a range (offset, size) of data received by a receiver.
 *
 * Class 2 CFDP deals with NAK, so received data must be tracked for receivers in order to generate
 * the NAK. The sender must also keep track of NAK requests and send new file data PDUs as a result.
 * (array size must be CF_NUM_CHANNELS)
 * CF_CHANNEL_NUM_RX_CHUNKS_PER_TRANSACTION is an array for each channel indicating the number of chunks per transaction
 * CF_CHANNEL_NUM_TX_CHUNKS_PER_TRANSACTION is an array for each channel indicating the number of chunks to keep track
 * of NAK requests from the receiver per transaction
 *
 *  @par Limits:
 *
 */
#define CF_CHANNEL_NUM_RX_CHUNKS_PER_TRANSACTION \
    {                                            \
        CF_NAK_MAX_SEGMENTS, CF_NAK_MAX_SEGMENTS \
    }

/**
 *  @brief TX chunks per transaction (per channel)
 *
 *  @par Description:
 *       Number of chunks per transaction per channel (TX).
 *
 *  @par Limits:
 *
 */
#define CF_CHANNEL_NUM_TX_CHUNKS_PER_TRANSACTION \
    {                                            \
        CF_NAK_MAX_SEGMENTS, CF_NAK_MAX_SEGMENTS \
    }

/**
 *  @brief Number of max commanded playback files per chan.
 *
 *  @par Description:
 *       This is the max number of outstanding ground commanded file transmits per channel.
 *
 *  @par Limits:
 *
 */
#define CF_MAX_COMMANDED_PLAYBACK_FILES_PER_CHAN (10)

/**
 *  @brief Max number of simultaneous file receives.
 *
 *  @par Description:
 *       Each channel can support this number of file receive transactions at a time.
 *
 *  @par Limits:
 *
 */
#define CF_MAX_SIMULTANEOUS_RX (5)

/* definitions that affect execution */

/**
 *  @brief Max number of commanded playback directories per channel.
 *
 *  @par Description:
 *       Each channel can support this number of ground commanded directory playbacks.
 *
 *  @par Limits:
 *
 */
#define CF_MAX_COMMANDED_PLAYBACK_DIRECTORIES_PER_CHAN (2)

/**
 *  @brief Number of transactions per playback directory.
 *
 *  @par Description:
 *       Each playback/polling directory operation will be able to have this
 *       many active transfers at a time pending or active.
 *
 *  @par Limits:
 *
 */
#define CF_NUM_TRANSACTIONS_PER_PLAYBACK (5)

/**
 *  @brief R2 CRC calc chunk size
 *
 *  @par Description
 *       R2 performs CRC calculation upon file completion in chunks. This is the size
 *       of the buffer. The larger the size the more stack will be used, but
 *       the faster it can go. The overall number of bytes calculated per wakeup
 *       is set in the configuration table.
 *
 *  @par Limits:
 *
 */
#define CF_R2_CRC_CHUNK_SIZE (1024)

}  // namespace Svc
}  // namespace Ccsds