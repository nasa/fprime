// ======================================================================
// \title  TimeConverter.hpp
// \author devin
// \brief  hpp file for TimeConverter component implementation class
// ======================================================================

#ifndef Svc_TimeConverter_HPP
#define Svc_TimeConverter_HPP

#include <limits>

#include "Fw/DataStructures/ArrayMap.hpp"
#include "Svc/Ports/TimeConverterPorts/TimeBasePairSerializableAc.hpp"
#include "Svc/TimeConverter/TimeConverterComponentAc.hpp"
#include "TimeConverterConfig/FppConstantsAc.hpp"

namespace Svc {

class TimeConverter final : public TimeConverterComponentBase {
  public:
    //! Microseconds in one second
    static constexpr I64 US_PER_SECOND = 1000000;

    //! Largest time representable by an Fw::Time, in microseconds
    static constexpr I64 MAX_TIME_US =
        static_cast<I64>(std::numeric_limits<U32>::max()) * US_PER_SECOND + (US_PER_SECOND - 1);

    // ----------------------------------------------------------------------
    // Component construction and destruction
    // ----------------------------------------------------------------------

    //! Construct TimeConverter object
    explicit TimeConverter(const char* const compName  //!< The component name
    );

    //! Destroy TimeConverter object
    ~TimeConverter();

  private:
    //! Outcome of storing an offset
    enum class StoreStatus {
        OK,         //!< the offset was stored
        INVALID,    //!< the pair or the offset itself was rejected
        TABLE_FULL  //!< no entry was available for a new pair
    };

    // ----------------------------------------------------------------------
    // Handler implementations for user-defined typed input ports
    // ----------------------------------------------------------------------

    //! Handler implementation for offsetUpdate
    void offsetUpdate_handler(FwIndexType portNum,           //!< The port number
                              const Svc::TimeOffset& offset  //!< The offset
                              ) override;

    //! Handler implementation for convertTime
    Svc::ConvertTimeStatus convertTime_handler(FwIndexType portNum,      //!< The port number
                                               const Fw::Time& in_time,  //!< The time to convert
                                               Fw::Time& out_time        //!< Carries the requested time base in,
                                                                         //!< and receives the converted time
                                               ) override;

    // ----------------------------------------------------------------------
    // Handler implementations for commands
    // ----------------------------------------------------------------------

    //! Handler implementation for command SET_OFFSET
    void SET_OFFSET_cmdHandler(FwOpcodeType opCode,   //!< The opcode
                               U32 cmdSeq,            //!< The command sequence number
                               const TimeBase& from,  //!< The time base converted from
                               const TimeBase& to,    //!< The time base converted to
                               I64 offset_us          //!< The offset
                               ) override;

    //! Handler implementation for command CLEAR_OFFSETS
    void CLEAR_OFFSETS_cmdHandler(FwOpcodeType opCode, U32 cmdSeq) override;

    //! Handler implementation for command GET_OFFSET
    void GET_OFFSET_cmdHandler(FwOpcodeType opCode, U32 cmdSeq, const TimeBase& from, const TimeBase& to) override;

    //! Handler implementation for command DUMP_OFFSETS
    void DUMP_OFFSETS_cmdHandler(FwOpcodeType opCode, U32 cmdSeq) override;

    // ----------------------------------------------------------------------
    // Helper functions
    // ----------------------------------------------------------------------

    //! Look up the offset converting a time from one base into another
    //! \return SUCCESS when an offset is stored for the pair
    Fw::Success lookupOffset(const TimeBase& from, const TimeBase& to, I64& offset_us) const;

    //! Store an offset, replacing an existing entry for the same pair
    //! \return the outcome, reported by event on failure
    StoreStatus storeOffset(const TimeBase& from, const TimeBase& to, I64 offset_us);

    //! Check that both ends of a pair denote a clock, reporting those that do not
    //! \return SUCCESS when both time bases are usable
    Fw::Success checkTimeBases(const TimeBase& from, const TimeBase& to);

    //! Offsets in microseconds added to a "lower" time to produce an "upper" time, one per pair
    Fw::ArrayMap<Svc::TimeBasePair, I64, Svc::TimeConverterCfg::MAX_OFFSET_ENTRIES> m_offsets;
};

}  // namespace Svc

#endif
