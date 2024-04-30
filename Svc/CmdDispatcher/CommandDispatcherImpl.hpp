/**
 * \file
 * \author T.Canham
 * \brief Component responsible for dispatching incoming commands to registered components
 *
 * \copyright
 * Copyright 2009-2015, by the California Institute of Technology.
 * ALL RIGHTS RESERVED.  United States Government Sponsorship
 * acknowledged.
 * <br /><br />
 */


#ifndef COMMANDDISPATCHERIMPL_HPP_
#define COMMANDDISPATCHERIMPL_HPP_

#include <Svc/CmdDispatcher/CommandDispatcherComponentAc.hpp>
#include <Os/Mutex.hpp>
#include <CommandDispatcherImplCfg.hpp>

namespace Svc {

    //! \class CommandDispatcherImpl
    //! \brief Command Dispatcher component class
    //!
    //! The command dispatcher takes incoming Fw::Com packets that contain
    //! encoded commands. It extracts the opcode and looks it up in a table
    //! that is populated by components at registration time. If a component
    //! is connected to the seqCmdStatus port with the same number
    //! as the port that submitted the command, the command status will be returned.

    class CommandDispatcherImpl : public CommandDispatcherComponentBase {
        public:
            //!  \brief Command Dispatcher constructor
            //!
            //!  The constructor initializes the state of the component.
            //!  In this component, the opcode dispatch and tracking tables
            //!  are initialized.
            //!
            //!  \param name the component instance name
            CommandDispatcherImpl(const char* name);
            //!  \brief Component initialization routine
            //!
            //!  The initialization function calls the initialization
            //!  routine for the base class.
            //!
            //!  \param queueDepth the depth of the message queue for the component
            void init(
                    NATIVE_INT_TYPE queueDepth, /*!< The queue depth*/
                    NATIVE_INT_TYPE instance /*!< The instance number*/
                    ); //!< initialization function
            //!  \brief Component destructor
            //!
            //!  The destructor for this component is empty
            virtual ~CommandDispatcherImpl();
        PROTECTED:
        PRIVATE:
            //!  \brief component command status handler
            //!
            //!  The command status handler is called when a component
            //!  reports the completion of a command.
            //!
            //!  \param portNum the number of the incoming port.
            //!  \param opCode the opcode of the completed command.
            //!  \param cmdSeq the sequence number assigned to the command when it was dispatched
            //!  \param response the completion status of the command
            void compCmdStat_handler(NATIVE_INT_TYPE portNum, FwOpcodeType opCode, U32 cmdSeq, const Fw::CmdResponse &response);
            //!  \brief component command buffer handler
            //!
            //!  The command buffer handler is called to submit a new
            //!  command packet to be decoded
            //!
            //!  \param portNum the number of the incoming port.
            //!  \param data the buffer containing the command.
            //!  \param context a user value returned with the status
            void seqCmdBuff_handler(NATIVE_INT_TYPE portNum, Fw::ComBuffer &data, U32 context);
            //!  \brief component command registration handler
            //!
            //!  The command registration handler is called to register
            //!  new opcodes. The port number called is used to indicate
            //!  which port should be used to dispatch the opcode.
            //!
            //!  \param portNum the number of the incoming port.
            //!  \param opCode the opcode being registered.
            void compCmdReg_handler(NATIVE_INT_TYPE portNum, FwOpcodeType opCode);
            //!  \brief component ping handler
            //!
            //!  The ping handler responds to messages to verify that the task
            //!  is still executing. Will call output ping port
            //!
            //!  \param portNum the number of the incoming port.
            //!  \param opCode the opcode being registered.
            //!  \param key the key value that is returned with the ping response
            void pingIn_handler(NATIVE_INT_TYPE portNum, U32 key);
            //!  \brief NO_OP command handler
            //!
            //!  A test command that does nothing
            //!
            //!  \param opCode the NO_OP opcode.
            //!  \param cmdSeq the assigned sequence number for the command
            void CMD_NO_OP_cmdHandler(FwOpcodeType opCode, U32 cmdSeq);
            //!  \brief NO_OP with string command handler
            //!
            //!  A test command that receives a string and sends an event
            //!  with the string as an argument
            //!
            //!  \param opCode the NO_OP_STRING opcode.
            //!  \param cmdSeq the assigned sequence number for the command
            //!  \param arg1 the string argument
            void CMD_NO_OP_STRING_cmdHandler(FwOpcodeType opCode, U32 cmdSeq, const Fw::CmdStringArg& arg1);
            //!  \brief A test command with different argument types
            //!
            //!  A test command that receives a set of arguments of different types
            //!
            //!  \param opCode the TEST_CMD_1 opcode.
            //!  \param cmdSeq the assigned sequence number for the command
            //!  \param arg1 the I32 argument
            //!  \param arg2 the F32 argument
            //!  \param arg3 the U8 argument
            void CMD_TEST_CMD_1_cmdHandler(FwOpcodeType opCode, U32 cmdSeq, I32 arg1, F32 arg2, U8 arg3);
            //!  \brief A command to clear the command tracking
            //!
            //!  This command will clear the table tracking the completion of commands.
            //!  It is meant to be used if the tracking table has gotten full because of
            //!  a software failure. It is dangerous in that it can clear a command
            //!  that a sequencer is waiting for.
            //!
            //!  \param opCode the CLEAR_TRACKING opcode.
            //!  \param cmdSeq the assigned sequence number for the command
            void CMD_CLEAR_TRACKING_cmdHandler(FwOpcodeType opCode, U32 cmdSeq);

            //! \struct DispatchEntry
            //! \brief table used to store opcode to port mappings
            //!
            //! The DispatchEntry table is used to map incoming opcodes to the port
            //! connected to the component that implements the opcode.
            //! As each command opcode is registered, a new entry is found
            //! in the table by checking for the "used" flag. The opcode
            //! member is set to the opcode, and the port member set to the
            //! port to dispatch to. When a new opcode is received for
            //! execution, the table is traversed until the opcode is located.

            struct DispatchEntry {
                    bool used; //!< if entry has been used yet
                    U32 opcode; //!< opcode of entry
                    NATIVE_INT_TYPE port; //!< which port the entry invokes
            } m_entryTable[CMD_DISPATCHER_DISPATCH_TABLE_SIZE]; //!< table of dispatch entries

            //! \struct SequenceTracker
            //! \brief table used to store opcode that are being executed
            //!
            //! The SequenceTracker table is used to track commands that are being executed
            //! but are not yet complete. When a new command opcode is received,
            //! the status port that would be used to report the completion status
            //! is checked. If it is connected, then an entry is placed in this table.
            //! The "used" flag is set, and the "seq" member is set to the
            //! assigned sequence number for the command. The "opCode" field is
            //! used for the opcode, and the "callerPort" field is used to store
            //! the port number of the caller so the status can be reported back to
            //! correct port.

            struct SequenceTracker {
                    bool used; //!< if this slot is used
                    U32 seq; //!< command sequence number
                    FwOpcodeType opCode; //!< opcode being tracked
                    U32 context; //!< context passed by user
                    NATIVE_INT_TYPE callerPort; //!< port command source port
            } m_sequenceTracker[CMD_DISPATCHER_SEQUENCER_TABLE_SIZE]; //!< sequence tracking port for command completions;

            U32 m_seq; //!< current command sequence number

            U32 m_numCmdsDispatched; //!< number of commands dispatched
            U32 m_numCmdErrors; //!< number of commands with an error

    };
}

#endif /* COMMANDDISPATCHERIMPL_HPP_ */
