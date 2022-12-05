// ======================================================================
// \title  Health.hpp
// \author Tim
// \brief  hpp file for Health component implementation class
//
// \copyright
// Copyright 2009-2015, by the California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
//
// ======================================================================

#include <Svc/Health/HealthComponentImpl.hpp>
#include <FpConfig.hpp>
#include <Fw/Types/Assert.hpp>

namespace Svc {

    // ----------------------------------------------------------------------
    // Construction, initialization, and destruction
    // ----------------------------------------------------------------------

    HealthImpl::HealthImpl(const char * const compName) :
            HealthComponentBase(compName),
            m_numPingEntries(0),
            m_key(0),
            m_watchDogCode(0),
            m_warnings(0),
            m_enabled(Fw::Enabled::ENABLED),
            queue_depth(0) {
        // clear tracker by disabling pings
        for (NATIVE_UINT_TYPE entry = 0;
                entry < FW_NUM_ARRAY_ELEMENTS(this->m_pingTrackerEntries);
                entry++) {
            this->m_pingTrackerEntries[entry].enabled = Fw::Enabled::DISABLED;
        }
    }

    void HealthImpl::init(const NATIVE_INT_TYPE queueDepth, const NATIVE_INT_TYPE instance) {
        HealthComponentBase::init(queueDepth, instance);
        this->queue_depth = queueDepth;

    }

    void HealthImpl::setPingEntries(PingEntry* pingEntries, NATIVE_INT_TYPE numPingEntries, U32 watchDogCode) {

        FW_ASSERT(pingEntries);
        // make sure not asking for more pings than ports
        FW_ASSERT(numPingEntries <= NUM_PINGSEND_OUTPUT_PORTS);

        this->m_numPingEntries = numPingEntries;
        this->m_watchDogCode = watchDogCode;

        // copy entries to private data
        for (NATIVE_INT_TYPE entry = 0; entry < numPingEntries; entry++) {
            FW_ASSERT(pingEntries[entry].warnCycles <= pingEntries[entry].fatalCycles, pingEntries[entry].warnCycles, pingEntries[entry].fatalCycles);
            this->m_pingTrackerEntries[entry].entry = pingEntries[entry];
            this->m_pingTrackerEntries[entry].cycleCount = 0;
            this->m_pingTrackerEntries[entry].enabled = Fw::Enabled::ENABLED;
            this->m_pingTrackerEntries[entry].key = 0;
        }
    }

    HealthImpl::~HealthImpl() {

    }

    // ----------------------------------------------------------------------
    // Handler implementations for user-defined typed input ports
    // ----------------------------------------------------------------------

    void HealthImpl::PingReturn_handler(const NATIVE_INT_TYPE portNum, U32 key) {
        // verify the key value
        if (key != this->m_pingTrackerEntries[portNum].key) {
            Fw::LogStringArg _arg = this->m_pingTrackerEntries[portNum].entry.entryName;
            this->log_FATAL_HLTH_PING_WRONG_KEY(_arg,key);
        } else {
            // reset the counter and clear the key
            this->m_pingTrackerEntries[portNum].cycleCount = 0;
            this->m_pingTrackerEntries[portNum].key = 0;
        }

    }

    void HealthImpl::Run_handler(const NATIVE_INT_TYPE portNum, NATIVE_UINT_TYPE context) {
        //dispatch messages
        for (NATIVE_UINT_TYPE i = 0; i < this->queue_depth; i++) {
            MsgDispatchStatus stat = this->doDispatch();
            if (MSG_DISPATCH_EMPTY == stat) {
                break;
            }
            FW_ASSERT(MSG_DISPATCH_OK == stat);
        }

        if (this->m_enabled == Fw::Enabled::ENABLED) {
            // cycle through ping table, pinging ports that are not awaiting a reply
            // for ports that are awaiting a reply, decrement their counters
            // and check for violations

            for (NATIVE_UINT_TYPE entry = 0; entry < this->m_numPingEntries; entry++) {
                if (Fw::Enabled::ENABLED == this->m_pingTrackerEntries[entry].enabled) {
                    // If clear entry
                    if (0 == this->m_pingTrackerEntries[entry].cycleCount) {
                        // start a ping
                        this->m_pingTrackerEntries[entry].key = this->m_key;
                        // send ping
                        this->PingSend_out(entry, this->m_pingTrackerEntries[entry].key);
                        // increment key
                        this->m_key++;
                        // increment cycles for the entry
                        this->m_pingTrackerEntries[entry].cycleCount++;
                    } else {
                        // check to see if it is at warning threshold
                        if (this->m_pingTrackerEntries[entry].cycleCount ==
                                this->m_pingTrackerEntries[entry].entry.warnCycles) {
                            Fw::LogStringArg _arg = this->m_pingTrackerEntries[entry].entry.entryName;
                            this->log_WARNING_HI_HLTH_PING_WARN(_arg);
                            this->tlmWrite_PingLateWarnings(++this->m_warnings);
                        } else {
                            // check for FATAL timeout value
                            if (this->m_pingTrackerEntries[entry].entry.fatalCycles ==
                                    this->m_pingTrackerEntries[entry].cycleCount) {
                                Fw::LogStringArg _arg = this->m_pingTrackerEntries[entry].entry.entryName;
                                this->log_FATAL_HLTH_PING_LATE(_arg);
                            }
                        } // if at warning or fatal threshold

                        this->m_pingTrackerEntries[entry].cycleCount++;
                    } // if clear entry
                } // if entry has ping enabled
            } // for each entry

            // do other specialized platform checks (e.g. VxWorks suspended tasks)
            this->doOtherChecks();

        } // If health checking is enabled

        // stroke watchdog.
        if (this->isConnected_WdogStroke_OutputPort(0)) {
            this->WdogStroke_out(0,this->m_watchDogCode);
        }
    }

    // ----------------------------------------------------------------------
    // Command handler implementations
    // ----------------------------------------------------------------------

    void HealthImpl::HLTH_ENABLE_cmdHandler(const FwOpcodeType opCode, U32 cmdSeq, Fw::Enabled enable) {
        this->m_enabled = enable;
        Fw::Enabled isEnabled = Fw::Enabled::DISABLED;
        if (enable == Fw::Enabled::ENABLED) {
            isEnabled = Fw::Enabled::ENABLED;
        }
        this->log_ACTIVITY_HI_HLTH_CHECK_ENABLE(isEnabled);
        this->cmdResponse_out(opCode,cmdSeq,Fw::CmdResponse::OK);
    }


    void HealthImpl::HLTH_PING_ENABLE_cmdHandler(const FwOpcodeType opCode, U32 cmdSeq, const Fw::CmdStringArg& entry, Fw::Enabled enable) {
        // check to see if entry is in range
        NATIVE_INT_TYPE entryIndex = this->findEntry(entry);

        if (-1 == entryIndex) {
            this->cmdResponse_out(opCode,cmdSeq,Fw::CmdResponse::VALIDATION_ERROR);
            return;
        }

        this->m_pingTrackerEntries[entryIndex].enabled = enable.e;
        Fw::Enabled isEnabled(Fw::Enabled::DISABLED);
        if (enable == Fw::Enabled::ENABLED) {
            isEnabled = Fw::Enabled::ENABLED;
        }
        Fw::LogStringArg arg;
        arg = entry;
        this->log_ACTIVITY_HI_HLTH_CHECK_PING(isEnabled,arg);
        this->cmdResponse_out(opCode,cmdSeq,Fw::CmdResponse::OK);
    }

    void HealthImpl::HLTH_CHNG_PING_cmdHandler(const FwOpcodeType opCode, U32 cmdSeq, const Fw::CmdStringArg& entry, U32 warningValue, U32 fatalValue) {
        // check to see if entry is in range
        NATIVE_INT_TYPE entryIndex = this->findEntry(entry);
        if (-1 == entryIndex) {
            this->cmdResponse_out(opCode,cmdSeq,Fw::CmdResponse::VALIDATION_ERROR);
            return;
        }

        //check to see if warningValue less than or equal to fatalValue
        if (warningValue > fatalValue) {
            Fw::LogStringArg arg;
            arg = entry;
            this->log_WARNING_HI_HLTH_PING_INVALID_VALUES(arg,warningValue,fatalValue);
            this->cmdResponse_out(opCode,cmdSeq,Fw::CmdResponse::VALIDATION_ERROR);
            return;
        }

        this->m_pingTrackerEntries[entryIndex].entry.warnCycles = warningValue;
        this->m_pingTrackerEntries[entryIndex].entry.fatalCycles = fatalValue;
        Fw::LogStringArg arg = entry;
        this->log_ACTIVITY_HI_HLTH_PING_UPDATED(arg,warningValue,fatalValue);
        this->cmdResponse_out(opCode,cmdSeq,Fw::CmdResponse::OK);
    }

    NATIVE_INT_TYPE HealthImpl::findEntry(const Fw::CmdStringArg& entry) {

        // walk through entries
        for (NATIVE_UINT_TYPE tableEntry = 0; tableEntry < NUM_PINGSEND_OUTPUT_PORTS; tableEntry++) {
            if (entry == this->m_pingTrackerEntries[tableEntry].entry.entryName) {
                return tableEntry;
            }
        }
        Fw::LogStringArg arg = entry;
        this->log_WARNING_LO_HLTH_CHECK_LOOKUP_ERROR(arg);

        return -1;
    }



} // end namespace Svc
