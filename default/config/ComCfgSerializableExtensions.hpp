#ifndef COMCFG_SERIALIZABLE_EXTENSIONS_HPP
#define COMCFG_SERIALIZABLE_EXTENSIONS_HPP

#include "default/config/ComCfgEnumAc.hpp"
#include "Fw/Types/Serializable.hpp"

/**
 * This file provides serialization extensions for auto-generated FPP types.
 * It bridges the gap between the old serialization methods and the new ones.
 */

namespace ComCfg {

// Add implementations for serializeTo and deserializeFrom
SerializeStatus APID::serializeTo(Fw::SerializeBufferBase& buffer) const {
    return buffer.serializeFrom(static_cast<FwEnumStoreType>(e));
}

SerializeStatus APID::deserializeFrom(Fw::SerializeBufferBase& buffer) {
    FwEnumStoreType val;
    SerializeStatus status = buffer.deserializeTo(val);
    if (status == Fw::FW_SERIALIZE_OK) {
        this->e = static_cast<t>(val);
    }
    return status;
}

} // namespace ComCfg

#endif // COMCFG_SERIALIZABLE_EXTENSIONS_HPP
