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
        skipOne(false)
  {}

  void SignalGen ::
    init(
        const NATIVE_INT_TYPE queueDepth,
        const NATIVE_INT_TYPE instance
    )
  {
      SignalGenComponentBase::init(queueDepth, instance);
  }

  SignalGen :: ~SignalGen() { }

  // ----------------------------------------------------------------------
  // Handler implementations
  // ----------------------------------------------------------------------

  F32 SignalGen :: generateSample(U32 ticks) {
      F32 val = 0.0f;
      if (this->skipOne) {
          return val;
      }
      // Samples per period
      F32 samplesPerPeriod = static_cast<F32>(this->sampleFrequency)/static_cast<F32>(this->signalFrequency);
      U32 halfSamplesPerPeriod = samplesPerPeriod/2;
      /* Signals courtesy of the open source Aquila DSP Library */
      switch(this->sigType.e) {
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

  void SignalGen :: schedIn_handler(
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
      this->ticks += 1;
  }

  void SignalGen :: SignalGen_Settings_cmdHandler(
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
      this->signalPhase     = Phase;
      this->sigType = SigType;

      // When the settings change, reset the history values
      for (U32 i = 0; i < SignalSet::SIZE; i++) {
          this->sigHistory[i] = 0.0f;
      }
      for (U32 i = 0; i < SignalPairSet::SIZE; i++) {
          this->sigPairHistory[i].settime(0.0f);
          this->sigPairHistory[i].setvalue(0.0f);
      }
      this->log_ACTIVITY_LO_SignalGen_SettingsChanged(this->signalFrequency, this->signalAmplitude, this->signalPhase, this->sigType);
      this->tlmWrite_Type(SigType);
      this->cmdResponse_out(opCode, cmdSeq, Fw::CmdResponse::OK);
  }

  void SignalGen :: SignalGen_Toggle_cmdHandler(
            FwOpcodeType opCode, /*!< The opcode*/
            U32 cmdSeq /*!< The command sequence number*/
        )
  {
      this->running = !this->running;
      this->ticks = 0;
      this->cmdResponse_out(opCode, cmdSeq, Fw::CmdResponse::OK);
  }



  void SignalGen :: SignalGen_Skip_cmdHandler(
      FwOpcodeType opCode, /*!< The opcode*/
      U32 cmdSeq /*!< The command sequence number*/
    )
  {
      this->skipOne = true;
      this->cmdResponse_out(opCode, cmdSeq, Fw::CmdResponse::OK);
  }

};
