/*
 * CircularBufferCfg.hpp:
 *
 * Configuration settings for the Types::CircularBuffer data type.
 */

#ifndef TYPES_CIRCULAR_BUFFER_CFG_HPP_
#define TYPES_CIRCULAR_BUFFER_CFG_HPP_

#include <Fw/FPrimeBasicTypes.hpp>

namespace Types {
namespace CircularBufferCfg {

// Size in bytes of the temporary stack buffer used to stage object (de)serialization when a slot
// wraps the end of the backing store. Slots larger than this size that wrap will assert. The
// default of 0 asserts on any wrapping slot, as wrap-around is rare (queue slots never wrap).
constexpr FwSizeType STAGING_BUFFER_SIZE = 0;

}  // namespace CircularBufferCfg
}  // namespace Types

#endif
