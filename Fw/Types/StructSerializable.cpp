#include <Fw/Types/StructSerializable.hpp>
#include <Fw/Types/Assert.hpp>

namespace Fw {

    template <class T> StructSerializable<T>::StructSerializable() {
    }

    template <class T> StructSerializable<T>::StructSerializable(const T& val) {
        this->m_val = val;
    }

    template <class T> StructSerializable<T>::StructSerializable(const T* val) {
        FW_ASSERT(val);
        this->m_val = *val;
    }

    template <class T> const T& StructSerializable<T>::operator=(const T& val) {
        this->m_val = val;
        return val;
    }

    template <class T> StructSerializable<T>::~StructSerializable() {
    }

    template <class T> void StructSerializable<T>::set(const T& val) {
        this->m_val = val;
    }

    template <class T> const T& StructSerializable<T>::get(void) {
        return this->m_val;
    }

    template <class T> Fw::SerializeStatus StructSerializable<T>::serialize(Fw::SerializeBufferBase& buffer) const {
        if (sizeof(T) > buffer.getBuffLeft()) {
            return FW_DESERIALIZE_SIZE_MISMATCH;
        }
        return buffer.serialize(static_cast<U8*>(&this->m_val),sizeof(T));
    }

    template <class T> Fw::SerializeStatus StructSerializable<T>::deserialize(Fw::SerializeBufferBase& buffer) {
        return buffer.deserialize(static_cast<U8*>(&this->m_val),sizeof(T));
    }

}

