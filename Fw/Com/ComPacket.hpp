/*
 * ComPacket.hpp
 *
 *  Created on: May 24, 2014
 *      Author: Timothy Canham
 */

#ifndef COMPACKET_HPP_
#define COMPACKET_HPP_

#include <Fw/Types/Serializable.hpp>
#include "config/APIDEnumAc.hpp"

// Packet format:
// |32-bit packet type|packet type-specific data|

namespace Fw {

// This type is defined in config/ComCfg.fpp
using ComPacketType = ComCfg::APID::T;

class ComPacket : public Serializable {
  public:
    ComPacket();
    virtual ~ComPacket();

  protected:
    ComPacketType m_type;
    SerializeStatus serializeBase(
        SerializeBufferBase& buffer) const;  // called by derived classes to serialize common fields
    SerializeStatus deserializeBase(
        SerializeBufferBase& buffer);  // called by derived classes to deserialize common fields
};

} /* namespace Fw */

#endif /* COMPACKET_HPP_ */
