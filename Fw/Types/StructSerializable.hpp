#ifndef STRUCT_SERIALIZABLE_HPP
#define STRUCT_SERIALIZABLE_HPP

#include <Fw/Types/BasicTypes.hpp>
#include <Fw/Types/Serializable.hpp>

namespace Fw {

    template <class T> class StructSerializable : public Fw::Serializable {
        public:

            enum {
                SERIALIZED_SIZE = sizeof(T) + sizeof(I32) // struct size plus int for storing size
            };

            StructSerializable();
            StructSerializable(const T& val);
            StructSerializable(const T* val);
            const T& operator=(const T& val);

            virtual ~StructSerializable();

            void set(const T& val);
            const T& get(void);

            Fw::SerializeStatus serialize(Fw::SerializeBufferBase& buffer) const;
            Fw::SerializeStatus deserialize(Fw::SerializeBufferBase& buffer);

        private:
            T m_val;

    };

}
#endif
