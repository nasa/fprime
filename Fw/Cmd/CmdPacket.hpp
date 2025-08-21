/*
 * CmdPacket.hpp
 *
 *  Created on: May 24, 2014
 *      Author: Timothy Canham
 */

#ifndef CMDPACKET_HPP_
#define CMDPACKET_HPP_

#include <Fw/Cmd/CmdArgBuffer.hpp>
#include <Fw/Com/ComPacket.hpp>

namespace Fw {

class CmdPacket : public ComPacket {
  public:
    CmdPacket();
    virtual ~CmdPacket();

    // New serialization interface methods
    SerializeStatus serializeTo(SerializeBufferBase& buffer) const;
    SerializeStatus deserializeFrom(SerializeBufferBase& buffer);

    FwOpcodeType getOpCode() const;
    CmdArgBuffer& getArgBuffer();

  protected:
    FwOpcodeType m_opcode;
    CmdArgBuffer m_argBuffer;
};

} /* namespace Fw */

#endif /* CMDPACKET_HPP_ */
