#ifndef FW_STATEMENT_BUFFER_HPP
#define FW_STATEMENT_BUFFER_HPP

#include <config/FpConfig.hpp>
#include <Fw/Types/Serializable.hpp>
#include <Fw/Types/SerIds.hpp>

namespace Fw {

    class StatementArgBuffer : public SerializeBufferBase {
        public:

            enum {
                SERIALIZED_TYPE_ID = FW_TYPEID_TLM_BUFF,
                SERIALIZED_SIZE = FW_STATEMENT_ARG_BUFFER_MAX_SIZE + sizeof(FwBuffSizeType)
            };

            StatementArgBuffer(const U8 *args, FwSizeType size);
            StatementArgBuffer();
            StatementArgBuffer(const StatementArgBuffer& other);
            virtual ~StatementArgBuffer();
            StatementArgBuffer& operator=(const StatementArgBuffer& other);

            Serializable::SizeType getBuffCapacity() const; // !< returns capacity, not current size, of buffer
            U8* getBuffAddr();
            const U8* getBuffAddr() const;
            bool operator==(const StatementArgBuffer& other) const;

            #if FW_SERIALIZABLE_TO_STRING
            void toString(Fw::StringBase& text) const;
            #endif
        PRIVATE:
            U8 m_bufferData[FW_STATEMENT_ARG_BUFFER_MAX_SIZE]; // command argument buffer
    };

}

#endif
