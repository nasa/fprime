// ====================================================================== 
// \title  CmdSequencerImpl.hpp
// \author Bocchino/Canham
// \brief  hpp file for CmdSequencer component implementation class
//
// \copyright
// Copyright (C) 2009-2016 California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged. Any commercial use must be negotiated with the Office
// of Technology Transfer at the California Institute of Technology.
// 
// This software may be subject to U.S. export control laws and
// regulations.  By accepting this document, the user agrees to comply
// with all U.S. export laws and regulations.  User has the
// responsibility to obtain export licenses, or other export authority
// as may be required before exporting such information to foreign
// countries or providing access to foreign persons.
// ====================================================================== 

#ifndef Svc_CmdSequencer_HPP
#define Svc_CmdSequencer_HPP

#include <Os/File.hpp>
#include <Os/ValidateFile.hpp>
#include <Svc/CmdSequencer/CmdSequencerComponentAc.hpp>
#include <Fw/Com/ComBuffer.hpp>
#include <Fw/Types/MemAllocator.hpp>

namespace Svc {

    class CmdSequencerComponentImpl: public CmdSequencerComponentBase {

            // ----------------------------------------------------------------------
            // Construction, initialization, and destruction
            // ----------------------------------------------------------------------

        public:

            //! Construct a CmdSequencer
            CmdSequencerComponentImpl(
#if FW_OBJECT_NAMES == 1
                    const char* compName //!< The component name
#else
                    void
#endif
                    );

            //! Initialize a CmdSequencer
            void init(const NATIVE_INT_TYPE queueDepth, //!< The queue depth
                    const NATIVE_INT_TYPE instance //!< The instance number
                    );

            //! optional - set a timeout. Sequence will quit if a command takes longer than the number of
            //! cycles in the timeout value.
            void setTimeout(NATIVE_UINT_TYPE seconds);

            //! Give the class a memory buffer. Should be called after constructor and init, but before task is spawned.
            void allocateBuffer(NATIVE_INT_TYPE identifier, Fw::MemAllocator& allocator, NATIVE_UINT_TYPE bytes);

            //! Return allocated buffer. Should be done during shutdown
            void deallocateBuffer(Fw::MemAllocator& allocator);

            //! Destroy a CmdDispatcherComponentBase
            ~CmdSequencerComponentImpl(void);

            // ----------------------------------------------------------------------
            // Handler implementations
            // ----------------------------------------------------------------------

        PRIVATE:

            // ----------------------------------------------------------------------
            // Types
            // ----------------------------------------------------------------------

            //! The run mode
            enum RunMode {
                STOPPED, RUNNING
            };

            //! The step mode
            enum StepMode {
                AUTO, MANUAL
            };

            enum CmdRecordDescriptor {
                ABSOLUTE, //!< Absolute time
                RELATIVE, //!< Relative time
                END_OF_SEQUENCE //!< end of sequence
            };

            // ----------------------------------------------------------------------
            // Private helper classes
            // ----------------------------------------------------------------------

            //! \class CmdRecord
            //! \brief A class representing an executing command record
            class CmdRecord : public Fw::Serializable {

                public:

                    //! Construct a CmdRecord object
                    CmdRecord(void) :
                        m_descriptor(END_OF_SEQUENCE) {
                    }

                    //! The command record descriptor
                    CmdRecordDescriptor m_descriptor;

                    //! Time tag for this command. NOTE: timeBase and context not filled in
                    Fw::Time m_timeTag;

                    //! The command associated with the command record
                    Fw::ComBuffer m_command;

                    Fw::SerializeStatus serialize(Fw::SerializeBufferBase& buffer) const; //!< serialize contents
                    Fw::SerializeStatus deserialize(Fw::SerializeBufferBase& buffer); //!< deserialize contents

            };

            //! \class Timer
            //! \brief A class representing a timer
            class Timer {

                PRIVATE:

                    //! The timer state
                    typedef enum {
                        SET, CLEAR
                    } State;

                public:

                    //! Construct a Timer object
                    Timer(void) :
                        m_state(CLEAR) {
                    }

                    //! Set the expiration time
                    void set(Fw::Time time //!< The time
                            ) {
                        this->m_state = SET;
                        this->expirationTime = time;
                    }

                    //! Clear the timer
                    void clear(void) {
                        this->m_state = CLEAR;
                    }

                    //! Determine whether the timer is expired at a given time
                    //! \return Yes or no
                    bool isExpiredAt(Fw::Time time //!< The time
                            ) {
                        if (this->m_state == CLEAR) {
                            return false;
                        } else if (Fw::Time::compare(this->expirationTime, time)
                                == Fw::Time::GT) {
                            return false;
                        }
                        return true;
                    }

                PRIVATE:

                    //! The timer state
                    State m_state;

                    //! The expiration time
                    Fw::Time expirationTime;

            };


            void CS_Run_cmdHandler(
            FwOpcodeType opCode, //!< The opcode
                    U32 cmdSeq, //!< The command sequence number
                    const Fw::CmdStringArg& fileName //!< The file name
                    );

            void CS_Validate_cmdHandler(
                    FwOpcodeType opCode, /*!< The opcode*/
                    U32 cmdSeq, /*!< The command sequence number*/
                    const Fw::CmdStringArg& fileName /*!< The name of the sequence file*/
                );

            void CS_Cancel_cmdHandler(
            FwOpcodeType opCode, //!< The opcode
                    U32 cmdSeq //!< The command sequence number
                    );

            //! Handler for input port cmdResponseIn
            //!
            void cmdResponseIn_handler(NATIVE_INT_TYPE portNum, //!< The port number
                    FwOpcodeType opcode, //!< The command opcode
                    U32 cmdSeq, //!< The command sequence number
                    Fw::CommandResponse response //!< The command response
                    );

            //! Handler for input port schedIn
            //!
            void schedIn_handler(NATIVE_INT_TYPE portNum, //!< The port number
                    NATIVE_UINT_TYPE order //!< The call order
                    );

            //! Handler for input port seqRunIn
            void seqRunIn_handler(
                   NATIVE_INT_TYPE portNum, /*!< The port number*/
                   Fw::EightyCharString &filename /*!< The sequence file*/
               );

            //! Handler for ping port
            void pingIn_handler(
                    NATIVE_INT_TYPE portNum, /*!< The port number*/
                    U32 key /*!< Value to return to pinger*/
                );

        PRIVATE:

            // ----------------------------------------------------------------------
            // Command handler implementations
            // ----------------------------------------------------------------------

            //! Handler for command CS_Start
            //! Start running a command sequence
            void CS_Start_cmdHandler(
            FwOpcodeType opcode, //!< The opcode
                    U32 cmdSeq //!< The command sequence number
                    );

            //! Handler for command CS_Step
            //! Perform one step in a command sequence.
            //! Valid only if SequenceRunner is in MANUAL run mode.
            void CS_Step_cmdHandler(
            FwOpcodeType opcode, //!< The opcode
                    U32 cmdSeq //!< The command sequence number
                    );

            //! Handler for command CS_Auto
            //! Set the run mode to AUTO.
            void CS_Auto_cmdHandler(
            FwOpcodeType opcode, //!< The opcode
                    U32 cmdSeq //!< The command sequence number
                    );

            //! Handler for command CS_Manual
            //! Set the run mode to MANUAL.
            void CS_Manual_cmdHandler(
            FwOpcodeType opcode, //!< The opcode
                    U32 cmdSeq //!< The command sequence number
                    );

            // ----------------------------------------------------------------------
            // Private helper methods
            // ----------------------------------------------------------------------

            // ----------------------------------------------------------------------
            // Helper methods
            // ----------------------------------------------------------------------

            //! Require a run mode
            //! \return Whether we are in the correct mode
            bool requireRunMode(RunMode mode); //!< The required mode

            //! Record an error
            void error(void);

            //! Record a completed command
            void commandComplete(const U32 opCode);

            //! Record an error in executing a sequence command
            void commandError(const U32 number, //!< The command number
                    const U32 opCode, //!< The command opcode
                    const U32 error //!< The error code
                    );

            //! Record a sequence complete event
            void sequenceComplete(void);

            //! Perform a Step command
            void performCmd_Step(void);

            //! Perform a Step command with an absolute time
            void performCmd_Step_ABSOLUTE(Fw::Time& currentTime //!< The time
                    );

            //! Perform a Step command with a relative time
            void performCmd_Step_RELATIVE(Fw::Time& currentTime //!< The time
                    );

            //! Emit a FileInvalid event
            void eventFileInvalid(FileReadStage stage, I32 error);

            //! Emit a FileReadError event
            void eventFileReadError(void);

            //! Emit a FileSizeError event
            void eventFileSizeError(U32 size);

            //! Emit a RecordInvalid event
            void eventRecordInvalid(U32 n, I32 error //!< The record number
                    );

            //! Emit a TimeBaseMismatch event
            void eventTimeBaseMismatch(
                    U32 currTimeBase, //!< The current time base
                    U32 seqTimeBase //!< The sequence file time base
                    );

            //! Emit a TimeContextMismatch event
            void eventTimeContextMismatch(
                    U32 currTimeContext, //!< The current time context
                    U32 seqTimeContxt //!< The sequence file time context
                    );

            //! Perform a Cancel command
            void performCmd_Cancel(void);

            //! Perform a Load command
            //! \return status (succeed or fail)
            bool performCmd_Load(const Fw::CmdStringArg& fileName); //!< The file name

            //! Read an open sequence file
            //! \return status (succeed or fail)
            bool readOpenSequenceFile(void);

            //! Read a sequence file
            //! \return status (succeed or fail)
            bool readSequenceFile(void);

            //! Validate a sequence buffer
            //! \return status (succeed or fail)
            bool validateBuffer(void);

            //! Some CRC helpers

            //! initialize CRC
            static void initCrc(U32 &crc);

            //! update CRC
            static void updateCRC(U32 &crc, const BYTE* buffer, NATIVE_UINT_TYPE bufferSize);

            //! finalize CRC
            static void finalizeCRC(U32 &crc);

            //! set command timeout timer
            void setCmdTimeout(const Fw::Time &currentTime);

            // ----------------------------------------------------------------------
            // Data
            // ----------------------------------------------------------------------

        PRIVATE:

            //! The deserialize buffer wrapper for the file contents
            Fw::ExternalSerializeBuffer m_seqBuffer;

            //! Number of records in the file
            U32 m_numRecords;

            //! Time base of file
            TimeBase m_timeBase;

            //! Context of file
            FwTimeContextStoreType m_timeContext;

            //! Computed CRC of file
            U32 m_fileCRC;

            //! The allocator ID
            NATIVE_INT_TYPE m_allocatorId;

            //! The sequence file name
            Fw::CmdStringArg m_fileName;
            //! Copy of file name for events
            Fw::LogStringArg m_logFileName;

            //! The sequence file
            Os::File m_sequenceFile;

            //! The number of Load commands executed
            U32 m_loadCmdCount;

            //! The number of Cancel commands executed
            U32 m_cancelCmdCount;

            //! The number of errors
            U32 m_errorCount;

            //! The queue depth
            NATIVE_INT_TYPE m_queueDepth;

            //! The run mode
            RunMode m_runMode;

            //! The step mode
            StepMode m_stepMode;

            //! The command record currently being processed
            CmdRecord m_cmdRecord;

            //! The command time timer
            Timer m_cmdTimer;

            //! The number of commands executed in this sequence
            U32 m_executedCount;

            //! The total number of commands executed across all sequences
            U32 m_totalExecutedCount;

            //! The total number of sequences completed
            U32 m_sequencesCompletedCount;

            //! Preamble for some checks
            void preamble(void);

            // some constants

            enum {
                SEQ_FILE_HEADER_SIZE = //!< sequence file header size
                    sizeof(U32) +
                    sizeof(U32) +
                    sizeof(FwTimeBaseStoreType) +
                    sizeof(FwTimeContextStoreType)
            };

            //! timeout value
            NATIVE_UINT_TYPE m_timeout;
            //! timeout timer
            Timer m_cmdTimeoutTimer;

    };

};

#endif
