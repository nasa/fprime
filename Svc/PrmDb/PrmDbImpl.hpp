/**
 * \file
 * \author T.Canham
 * \brief Component for managing parameters
 *
 * \copyright
 * Copyright 2009-2015, by the California Institute of Technology.
 * ALL RIGHTS RESERVED.  United States Government Sponsorship
 * acknowledged.
 * <br /><br />
 */

#ifndef PRMDBIMPL_HPP_
#define PRMDBIMPL_HPP_

#include <Svc/PrmDb/PrmDbComponentAc.hpp>
#include <PrmDbImplCfg.hpp>
#include <Fw/Types/String.hpp>
#include <Os/Mutex.hpp>

namespace Svc {

    //! \class PrmDbImpl
    //! \brief Component class for managing parameters
    //!
    //! This component supports storing, setting and saving of serialized parameters
    //! for components.
    //!

    class PrmDbImpl : public PrmDbComponentBase {
        public:

            friend class PrmDbImplTester;

            //!  \brief PrmDb constructor
            //!
            //!  The constructor for the PrmDbImpl class.
            //!   The constructor clears the database and stores
            //!   the file name for opening later.
            //!
            //!  \param name component instance name
            PrmDbImpl(const char* name);

            //!  \brief PrmDb initialization function
            //!
            //!  The initialization function for the component creates the message
            //!  queue and initializes the component base classes.
            //!
            //!  \param queueDepth queue depth for messages
            //!  \param instance instance of component, if more than one is needed.

            void init(NATIVE_INT_TYPE queueDepth, NATIVE_INT_TYPE instance);

            //!  \brief PrmDb configure method
            //!
            //!  The configure method stores the file name for opening later.
            //!
            //!  \param file file where parameters are stored.
            void configure(const char* file);


            //!  \brief PrmDb file read function
            //!
            //!  The readFile function reads the set of parameters from the file passed in to
            //!  the constructor.
            //!
            void readParamFile(); // NOTE: Assumed to run at initialization time. No guard of data structure.

            //!  \brief PrmDb destructor
            //!
            virtual ~PrmDbImpl();
        protected:
        private:
            //!  \brief PrmDb parameter get handler
            //!
            //!  This function retrieves a parameter value from the loaded set of stored parameters
            //!
            //!  \param portNum input port number. Should always be zero
            //!  \param id identifier for parameter being used.
            //!  \param val buffer where value is placed.
            //!  \return status of retrieval. PARAM_VALID = successful read, PARAM_INVALID = unsuccessful read
            Fw::ParamValid getPrm_handler(NATIVE_INT_TYPE portNum, FwPrmIdType id, Fw::ParamBuffer &val);
            //!  \brief PrmDb parameter set handler
            //!
            //!  This function updates the value of the parameter stored in RAM. The PRM_SAVE_FILE
            //!  must be called to save the value to a file.
            //!
            //!  \param portNum input port number. Should always be zero
            //!  \param id identifier for parameter being used.
            //!  \param val buffer where value to be saved is stored.
            void setPrm_handler(NATIVE_INT_TYPE portNum, FwPrmIdType id, Fw::ParamBuffer &val);

            //!  \brief component ping handler
            //!
            //!  The ping handler responds to messages to verify that the task
            //!  is still executing. Will call output ping port
            //!
            //!  \param portNum the number of the incoming port.
            //!  \param opCode the opcode being registered.
            //!  \param key the key value that is returned with the ping response

            void pingIn_handler(NATIVE_INT_TYPE portNum, U32 key);
            //!  \brief PrmDb PRM_SAVE_FILE command handler
            //!
            //!  This function saves the parameter values stored in RAM to the file
            //!  specified in the constructor. Any updates to parameters are not saved
            //!  until this function is called.
            //!
            //!  \param opCode The opcode of this commands
            //!  \param cmdSeq The sequence number of the command
            void PRM_SAVE_FILE_cmdHandler(FwOpcodeType opCode, U32 cmdSeq);

            //!  \brief PrmDb clear database function
            //!
            //!  This function clears all entries from the RAM database
            //!

            void clearDb(); //!< clear the parameter database

            Fw::String m_fileName; //!< filename for parameter storage

            struct t_dbStruct {
                bool used; //!< whether slot is being used
                FwPrmIdType id; //!< the id being stored in the slot
                Fw::ParamBuffer val; //!< the serialized value of the parameter
            } m_db[PRMDB_NUM_DB_ENTRIES];

    };
}

#endif /* PRMDBIMPL_HPP_ */
