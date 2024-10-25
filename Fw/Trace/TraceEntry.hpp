/*
 * TraceEntry.hpp
 *
 *      Author: sreddy
 */

#ifndef TRACE_ENTRY_HPP_
#define TRACE_ENTRY_HPP_

#include <Fw/Com/ComPacket.hpp>
#include <Fw/Trace/TraceBuffer.hpp>
#include <Fw/Time/Time.hpp>

namespace Fw {

    class TraceEntry : public ComPacket {
        public:

            TraceEntry();
            virtual ~TraceEntry();

            SerializeStatus serialize(SerializeBufferBase& buffer) const; //!< serialize contents
            SerializeStatus deserialize(SerializeBufferBase& buffer);

            void setId(FwTraceIdType id);
            void setTraceBuffer(const TraceBuffer& buffer);
            void setTimeTag(const Fw::Time& timeTag);

            FwTraceIdType getId();
            Fw::Time& getTimeTag();
            TraceBuffer& getTraceBuffer();

        protected:
            FwTraceIdType m_id; // !< Channel id
            Fw::Time m_timeTag; // !< time tag
            TraceBuffer m_traceBuffer; // !< serialized argument data
    };

} /* namespace Fw */

#endif /* TRACE_ENTRY_HPP_ */
