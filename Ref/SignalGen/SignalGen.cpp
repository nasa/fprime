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
#include <Fw/Types/SerialBuffer.hpp>
#include <Ref/SignalGen/SignalGen.hpp>
#include <Utils/Hash/Hash.hpp>
#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <functional>

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
      RUNNING(false),
      SKIP_NEXT(false),
      sampleFrequency(25),
      signalFrequency(1),
      signalAmplitude(0),
      signalPhase(0),
      sample(0),
      SignalType(SINE)

  {

  }

  void SignalGen ::
    init(
        const NATIVE_INT_TYPE queueDepth,
        const NATIVE_INT_TYPE instance
    )
  {
    SignalGenComponentBase::init(queueDepth, instance);
  }

  SignalGen :: ~SignalGen(void) { }

  // ----------------------------------------------------------------------
  // Handler implementations
  // ----------------------------------------------------------------------

  void SignalGen :: schedIn_handler(
      NATIVE_INT_TYPE portNum, /*!< The port number*/
      NATIVE_UINT_TYPE context /*!< The call order*/
  )
  {
    this->doDispatch();

    if(this->SKIP_NEXT){
      this->SKIP_NEXT = false;
      this->tlmWrite_SignalGen_Output(0);
      return;
    }
    

    if(this->RUNNING){

      switch(this->SignalType){
        /*

          Signals courtesy of the open source Aquila DSP Library

        */

        case TRIANGLE:
        {

          double samplesPerPeriod = this->sampleFrequency / this->signalFrequency;
          double risingLength = samplesPerPeriod / 2;
          double m = this->signalAmplitude / risingLength;

          if(this->sample < risingLength){
            this->tlmWrite_SignalGen_Output(m * this->sample);
          }else{
            this->tlmWrite_SignalGen_Output(0);
            this->sample = 0;
          }
          this->sample++;

          break;
        }

        case SINE:
        {
          double normalizedFrequency = this->signalFrequency / static_cast<double>(this->sampleFrequency);
          F32 val = static_cast<F32>(this->signalAmplitude) * std::sin( (2.0 * M_PI * normalizedFrequency * this->sample) + (this->signalPhase * 2.0 * M_PI) );
          this->tlmWrite_SignalGen_Output(val);

          U32 samplesPerPeriod = static_cast<int>(1/normalizedFrequency);
          if(++this->sample >= samplesPerPeriod){this->sample = 0;}
          break;
        }

        case SQUARE:
        {
          double duty_cycle = 0.5;
          U32 samplesPerPeriod = this->sampleFrequency / this->signalFrequency;
          double positiveLength = static_cast<std::size_t>(duty_cycle * samplesPerPeriod);
          F32 val = this->signalAmplitude * (this->sample < positiveLength ? 1 : -1);
          if(++this->sample > samplesPerPeriod){this->sample = 0;}
          this->tlmWrite_SignalGen_Output(val);
          break;
        }

        case NOISE:
        {
          F32 val = this->signalAmplitude * (std::rand() / static_cast<double>(RAND_MAX));
          this->tlmWrite_SignalGen_Output(val);
        }

      }
    }


    }
  

  void SignalGen :: SignalGen_Settings_cmdHandler(
        FwOpcodeType opCode, /*!< The opcode*/
        U32 cmdSeq, /*!< The command sequence number*/
        U32 Frequency,
        U32 Amplitude,
        U32 Phase
    )
  {
    this->signalFrequency = Frequency;
    this->signalAmplitude = Amplitude;
    this->signalPhase     = Phase;

    this->cmdResponse_out(opCode, cmdSeq, Fw::COMMAND_OK);
    //this->log_ACTIVITY_LO_SignalGen_SettingsChanged(this->signalFrequency, this->signalAmplitude, this->signalPhase);
  }

  void SignalGen :: SignalGen_Toggle_cmdHandler(
            FwOpcodeType opCode, /*!< The opcode*/
            U32 cmdSeq /*!< The command sequence number*/
        )
  {
    this->RUNNING = !this->RUNNING;
    this->cmdResponse_out(opCode, cmdSeq, Fw::COMMAND_OK);
  }



    void SignalGen :: SignalGen_Skip_cmdHandler(
        FwOpcodeType opCode, /*!< The opcode*/
        U32 cmdSeq /*!< The command sequence number*/
    )
    {
      this->SKIP_NEXT = true;
    }








};
