// ======================================================================
// \title  SignalGen.hpp
// \author bocchino
// \brief  hpp file for SequenceFileLoader component implementation class
//
// \copyright
// Copyright (C) 2009-2016 California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
//
// ======================================================================

#ifndef Svc_SignalGen_HPP
#define Svc_SignalGen_HPP

#include <Fw/Types/ByteArray.hpp>
#include <Fw/Types/ConstByteArray.hpp>
#include <Os/File.hpp>
#include <Os/ValidateFile.hpp>
#include <Ref/SignalGen/SignalGenComponentAc.hpp>
#include <cmath>

namespace Ref {

  class SignalGen :
    public SignalGenComponentBase
  {

    private:

        void schedIn_handler(
            NATIVE_INT_TYPE portNum, /*!< The port number*/
            U32 context /*!< The call order*/
        );

        void SignalGen_Settings_cmdHandler(
        FwOpcodeType opCode, /*!< The opcode*/
        U32 cmdSeq, /*!< The command sequence number*/
        U32 Frequency,
        F32 Amplitude,
        F32 Phase,
        Ref::SignalType SigType
        );

        void SignalGen_Toggle_cmdHandler(
            FwOpcodeType opCode, /*!< The opcode*/
            U32 cmdSeq /*!< The command sequence number*/
        );
        void SignalGen_Skip_cmdHandler(
        FwOpcodeType opCode, /*!< The opcode*/
        U32 cmdSeq /*!< The command sequence number*/
        );
        void SignalGen_GenerateArray_cmdHandler(
        FwOpcodeType opCode, /*!< The opcode*/
        U32 cmdSeq /*!< The command sequence number*/
        );

    public:
        //! Construct a SignalGen
        SignalGen(
            const char* compName //!< The component name
        );



        //! Initialize a SignalGen
        void init(
            const NATIVE_INT_TYPE queueDepth, //!< The queue depth
            const NATIVE_INT_TYPE instance //!< The instance number
        );

        //! Destroy a SignalGen
        ~SignalGen();

    private:
        // Generate the next sample internal helper
        F32 generateSample(U32 ticks);

        // Member variables
        U32 sampleFrequency;
        U32 signalFrequency;
        F32 signalAmplitude;
        F32 signalPhase;
        U32 ticks;
        SignalType sigType;
        SignalSet sigHistory;
        SignalPairSet sigPairHistory;
        bool running;
        bool skipOne;

  };
};
#endif
