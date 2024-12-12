// ======================================================================
// \title  SequenceFileLoader.cpp
// \author bocchino
// \brief  cpp file for SequenceFileLoader component implementation class
//
// \copyright
// Copyright (C) 2009-2016 California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
//
// ======================================================================

#include <Fw/Types/Assert.hpp>
#include <Ref/SignalGen/SignalGen.hpp>
#include <cmath>
#include <cstdlib>

// TKC - don't know why it's undefined in VxWorks
#ifdef TGT_OS_TYPE_VXWORKS
#define M_PI (22.0/7.0)
#endif

namespace Ref {

    // ----------------------------------------------------------------------
    // Construction, initialization, and destruction
    // ----------------------------------------------------------------------

    SignalGen ::
        SignalGen(const char* name) :
        SignalGenComponentBase(name),
        sampleFrequency(25),
        signalFrequency(1),
        signalAmplitude(0.0f),
        signalPhase(0.0f),
        ticks(0),
        sigType(SignalType::SINE),
        sigHistory(),
        sigPairHistory(),
        running(false),
        skipOne(false),
        m_dpInProgress(false),
        m_numDps(0),
        m_currDp(0),
        m_dpPriority(0)
    {}


    SignalGen :: ~SignalGen() { }

    // ----------------------------------------------------------------------
    // Handler implementations
    // ----------------------------------------------------------------------

    F32 SignalGen::generateSample(U32 ticks) {
        F32 val = 0.0f;
        if (this->skipOne) {
            return val;
        }
        // Samples per period
        F32 samplesPerPeriod = static_cast<F32>(this->sampleFrequency) / static_cast<F32>(this->signalFrequency);
        U32 halfSamplesPerPeriod = samplesPerPeriod / 2;
        /* Signals courtesy of the open source Aquila DSP Library */
        switch (this->sigType.e) {
        case SignalType::TRIANGLE:
        {
            F32 m = this->signalAmplitude / static_cast<F32>(halfSamplesPerPeriod);
            val = m * static_cast<F32>(ticks % halfSamplesPerPeriod);
            break;
        }
        case SignalType::SINE:
        {
            F32 normalizedFrequency = 1.0f / samplesPerPeriod;
            val = this->signalAmplitude * std::sin((2.0 * M_PI * normalizedFrequency *
                static_cast<F32>(ticks)) + (this->signalPhase * 2.0 * M_PI));
            break;
        }
        case SignalType::SQUARE:
        {
            val = this->signalAmplitude * ((ticks % static_cast<U32>(samplesPerPeriod) < halfSamplesPerPeriod) ? 1.0f : -1.0f);
            break;
        }
        case SignalType::NOISE:
        {
            val = this->signalAmplitude * (std::rand() / static_cast<double>(RAND_MAX));
            break;
        }
        default:
            FW_ASSERT(0); // Should never happen
        }
        return val;
    }

    void SignalGen::schedIn_handler(
        NATIVE_INT_TYPE portNum, /*!< The port number*/
        U32 context /*!< The call order*/
    )
    {
        F32 value = 0.0f;
        // This is a queued component, so it must intentionally run the dispatch of commands and queue processing on this
        // synchronous scheduled call
        this->doDispatch();

        // This short-circuits when the signal generator is not running
        if (not this->running) {
            return;
        }
        // Allows for skipping a single reading of the signal
        if (not this->skipOne) {
            value = this->generateSample(this->ticks);
        }
        this->skipOne = false;

        // Build our new types
        SignalPair pair = SignalPair(this->ticks, value);

        // Shift and assign our array types
        for (U32 i = 1; i < this->sigHistory.SIZE; i++) {
            this->sigHistory[i - 1] = this->sigHistory[i];
            this->sigPairHistory[i - 1] = this->sigPairHistory[i];
        }
        this->sigHistory[this->sigHistory.SIZE - 1] = value;
        this->sigPairHistory[this->sigPairHistory.SIZE - 1] = pair;

        // Composite structure
        SignalInfo sigInfo(this->sigType, this->sigHistory, this->sigPairHistory);

        // Write all signals
        this->tlmWrite_Type(this->sigType);
        this->tlmWrite_Output(value);
        this->tlmWrite_PairOutput(pair);
        this->tlmWrite_History(this->sigHistory);
        this->tlmWrite_PairHistory(this->sigPairHistory);
        this->tlmWrite_Info(sigInfo);

        // if a Data product is being generated, store a record
        if (this->m_dpInProgress) {
            // printf("DP1: %u %u %lu %u\n",
            //     this->m_dpContainer.getBuffer().getSize(),
            //     SignalInfo::SERIALIZED_SIZE,
            //     this->m_dpContainer.getDataSize(),
            //     this->m_dpContainer.getBuffer().getSerializeRepr().getBuffLeft()
            //     );
            Fw::SerializeStatus stat = this->m_dpContainer.serializeRecord_DataRecord(sigInfo);
            this->m_currDp++;
            this->m_dpBytes += SignalInfo::SERIALIZED_SIZE;
            // printf("DP2: %u %u %lu\n",this->m_dpContainer.getBuffer().getSize(),this->m_dpBytes,this->m_dpContainer.getDataSize());
            // check for full data product
            if (Fw::SerializeStatus::FW_SERIALIZE_NO_ROOM_LEFT == stat) {
                this->log_WARNING_LO_DpRecordFull(this->m_currDp,this->m_dpBytes);
                this->cleanupAndSendDp();
            } else if (this->m_currDp == this->m_numDps) { // if we reached the target number of DPs
                this->log_ACTIVITY_LO_DpComplete(this->m_numDps,this->m_dpBytes);
                this->cleanupAndSendDp();
            }

            this->tlmWrite_DpBytes(this->m_dpBytes);
            this->tlmWrite_DpRecords(this->m_currDp);
        }

        this->ticks += 1;
    }

    void SignalGen::Settings_cmdHandler(
        FwOpcodeType opCode, /*!< The opcode*/
        U32 cmdSeq, /*!< The command sequence number*/
        U32 Frequency,
        F32 Amplitude,
        F32 Phase,
        Ref::SignalType SigType
    )
    {
        this->signalFrequency = Frequency;
        this->signalAmplitude = Amplitude;
        this->signalPhase = Phase;
        this->sigType = SigType;

        // When the settings change, reset the history values
        for (U32 i = 0; i < SignalSet::SIZE; i++) {
            this->sigHistory[i] = 0.0f;
        }
        for (U32 i = 0; i < SignalPairSet::SIZE; i++) {
            this->sigPairHistory[i].settime(0.0f);
            this->sigPairHistory[i].setvalue(0.0f);
        }
        this->log_ACTIVITY_LO_SettingsChanged(this->signalFrequency, this->signalAmplitude, this->signalPhase, this->sigType);
        this->tlmWrite_Type(SigType);
        this->cmdResponse_out(opCode, cmdSeq, Fw::CmdResponse::OK);
    }

    void SignalGen::Toggle_cmdHandler(
        FwOpcodeType opCode, /*!< The opcode*/
        U32 cmdSeq /*!< The command sequence number*/
    )
    {
        this->running = !this->running;
        this->ticks = 0;
        this->cmdResponse_out(opCode, cmdSeq, Fw::CmdResponse::OK);
    }

    void SignalGen::Skip_cmdHandler(
        FwOpcodeType opCode, /*!< The opcode*/
        U32 cmdSeq /*!< The command sequence number*/
    )
    {
        this->skipOne = true;
        this->cmdResponse_out(opCode, cmdSeq, Fw::CmdResponse::OK);
    }

    void SignalGen::Dp_cmdHandler(
        FwOpcodeType opCode,
        U32 cmdSeq,
        Ref::SignalGen_DpReqType reqType,
        U32 records,
        U32 priority
    )
    {
        // at least one record
        if (0 == records) {
            this->log_WARNING_HI_InSufficientDpRecords();
            this->cmdResponse_out(opCode, cmdSeq, Fw::CmdResponse::VALIDATION_ERROR);
            return;
        }

        // make sure DPs are available
        if (
            not this->isConnected_productGetOut_OutputPort(0)
        ) {
            this->log_WARNING_HI_DpsNotConnected();
            this->cmdResponse_out(opCode, cmdSeq, Fw::CmdResponse::EXECUTION_ERROR);
            return;
        }

        // get DP buffer. Use sync or async request depending on 
        // requested type
        FwSizeType dpSize = records*(SignalInfo::SERIALIZED_SIZE + sizeof(FwDpIdType));
        this->m_numDps = records;
        this->m_currDp = 0;
        this->m_dpPriority = static_cast<FwDpPriorityType>(priority);
        this->log_ACTIVITY_LO_DpMemRequested(dpSize);
        if (Ref::SignalGen_DpReqType::IMMEDIATE ==  reqType) {
            Fw::Success stat = this->dpGet_DataContainer(dpSize,this->m_dpContainer);
            // make sure we got the memory we wanted
            if (Fw::Success::FAILURE == stat) {
                this->log_WARNING_HI_DpMemoryFail();
                this->cmdResponse_out(opCode, cmdSeq, Fw::CmdResponse::EXECUTION_ERROR);
            } else {
                this->m_dpInProgress = true;
                this->log_ACTIVITY_LO_DpStarted(records);
                this->log_ACTIVITY_LO_DpMemReceived(this->m_dpContainer.getBuffer().getSize());
                this->cmdResponse_out(opCode, cmdSeq, Fw::CmdResponse::OK);
                // override priority with requested priority
                this->m_dpContainer.setPriority(this->m_dpPriority);
            }
        } else if (Ref::SignalGen_DpReqType::ASYNC == reqType) {
            this->dpRequest_DataContainer(dpSize);
            this->cmdResponse_out(opCode, cmdSeq, Fw::CmdResponse::OK);
        } else {
            // should never get here
            FW_ASSERT(0,reqType.e);
        }

    }

    void SignalGen::cleanupAndSendDp() {
        this->dpSend(this->m_dpContainer);
        this->m_dpInProgress = false;
        this->m_dpBytes = 0;
        this->m_numDps = 0;
        this->m_currDp = 0;
    }



    // ----------------------------------------------------------------------
    // Handler implementations for data products
    // ----------------------------------------------------------------------

    void SignalGen ::
        dpRecv_DataContainer_handler(
            DpContainer& container,
            Fw::Success::T status
        )
    {

        // Make sure we got the buffer we wanted or quit
        if (Fw::Success::SUCCESS == status) {
            this->m_dpContainer = container;
            this->m_dpInProgress = true;
            // set previously requested priority
            this->m_dpContainer.setPriority(this->m_dpPriority);
            this->log_ACTIVITY_LO_DpStarted(this->m_numDps);
        } else {
            this->log_WARNING_HI_DpMemoryFail();
            // cleanup
            this->m_dpInProgress = false;
            this->m_dpBytes = 0;
            this->m_numDps = 0;
            this->m_currDp = 0;
        }
    }

};
