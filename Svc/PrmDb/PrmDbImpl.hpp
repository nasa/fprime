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

#include <Fw/Types/String.hpp>
#include <Os/Mutex.hpp>
#include <Svc/PrmDb/PrmDbComponentAc.hpp>
#include <Svc/PrmDb/PrmDb_PrmDbFileLoadStateEnumAc.hpp>
#include <Svc/PrmDb/PrmDb_PrmDbTypeEnumAc.hpp>
#include <config/PrmDbImplCfg.hpp>
#include <cstring>

namespace Svc {

typedef PrmDb_PrmWriteError PrmWriteError;
typedef PrmDb_PrmReadError PrmReadError;
typedef PrmDb_PrmDbType PrmDbType;
typedef PrmDb_PrmDbFileLoadState PrmDbFileLoadState;

//! \class PrmDbImpl
//! \brief Component class for managing parameters
//!
//! This component supports storing, setting and saving of serialized parameters
//! for components.
//!

class PrmDbImpl final : public PrmDbComponentBase {
    friend class PrmDbTester;

  public:
    //!  \brief PrmDb constructor
    //!
    //!  The constructor for the PrmDbImpl class.
    //!   The constructor clears the database and initiates the internal state.
    //!
    //!  \param name component instance name
    PrmDbImpl(const char* name);

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
    void readParamFile();  // NOTE: Assumed to run at initialization time. No guard of data structure.

    //!  \brief PrmDb destructor
    //!
    virtual ~PrmDbImpl();

    // Parameter update status for an individual parameter update or add
    enum PrmUpdateType {
        NO_SLOTS,       //!< No slots available to add new parameter
        PARAM_ADDED,    //!< Parameter added to database
        PARAM_UPDATED,  //!< Parameter already in database, updated parameter
        MAX_PARAM_UPDATE_TYPES
    };

    // Enum to return status of parameter file load
    enum PrmLoadStatus {
        SUCCESS,  //!< File load successful
        ERROR,    //!< File load error
    };

  protected:
  private:
    Fw::String m_fileName;  //!< filename for parameter storage

    PrmDbFileLoadState m_state;  // Current file load state of the parameter database

    // Structure for a single parameter entry
    struct t_dbStruct {
        bool used;            //!< whether slot is being used
        FwPrmIdType id;       //!< the id being stored in the slot
        Fw::ParamBuffer val;  //!< the serialized value of the parameter

        bool operator==(const t_dbStruct& other) const {
            if (used != other.used)
                return false;
            if (id != other.id)
                return false;
            // Compare lengths first
            if (val.getSize() != other.val.getSize())
                return false;
            // Compare buffer contents
            return std::memcmp(val.getBuffAddr(), other.val.getBuffAddr(), val.getSize()) == 0;
        }
    };

    // Pointers to the active and staging databases
    // These point to the actual storage arrays below
    // The active database is the ONLY one used for getting parameters
    // The staging database is used for loading parameters from a file
    // when commanded. Upon reading the file, the parameters are "staged"
    // into the staging database, and then committed to the active database
    // when a commit command is received.
    t_dbStruct* m_activeDb;   //!< Pointer to the active database
    t_dbStruct* m_stagingDb;  //!< Pointer to the staging database

    // Actual storage for the active and staging databases
    t_dbStruct m_dbStore1[PRMDB_NUM_DB_ENTRIES];
    t_dbStruct m_dbStore2[PRMDB_NUM_DB_ENTRIES];

    //! ----------------------------------------------------------------------
    //! Port & Command Handlers
    //! ----------------------------------------------------------------------

    //! \brief Read a parameter file and store parameter values into a database
    //!
    //!  This method reads a parameter file and applies the values to the specified database
    //!  (i.e. active or staging).
    //!
    //!
    //!  \param fileName The name of the parameter file to read
    //!  \param dbType The type of database to read into  (active or staging)
    //!  \return status success (True) / failure(False)
    PrmLoadStatus readParamFileImpl(const Fw::StringBase& fileName, PrmDbType dbType);

    //!  \brief PrmDb parameter get handler
    //!
    //!  This function retrieves a parameter value from the loaded set of stored parameters
    //!  It ALWAYS searches the active database, only
    //!
    //!  \param portNum input port number. Should always be zero
    //!  \param id identifier for parameter being used.
    //!  \param val buffer where value is placed.
    //!  \return status of retrieval. PARAM_VALID = successful read, PARAM_INVALID = unsuccessful read
    Fw::ParamValid getPrm_handler(FwIndexType portNum, FwPrmIdType id, Fw::ParamBuffer& val);

    //!  \brief PrmDb parameter set handler
    //!
    //!  This function updates the value of the parameter stored in RAM. The PRM_SAVE_FILE
    //!  must be called to save the value to a file.
    //!
    //!  \param portNum input port number. Should always be zero
    //!  \param id identifier for parameter being used.
    //!  \param val buffer where value to be saved is stored.
    void setPrm_handler(FwIndexType portNum, FwPrmIdType id, Fw::ParamBuffer& val);

    //!  \brief PrmDb parameter add or update (set) helper
    //!
    //!  This function does the underlying parameter update
    //!
    //!  \param id identifier for parameter being used.
    //!  \param val buffer where value to be saved is stored.
    //!  \param dbType The type of database to read into (active or staging)
    PrmDbImpl::PrmUpdateType updateAddPrmImpl(FwPrmIdType id, Fw::ParamBuffer& val, PrmDbType prmDbType);

    //!  \brief component ping handler
    //!
    //!  The ping handler responds to messages to verify that the task
    //!  is still executing. Will call output ping port
    //!
    //!  \param portNum the number of the incoming port.
    //!  \param opCode the opcode being registered.
    //!  \param key the key value that is returned with the ping response
    void pingIn_handler(FwIndexType portNum, U32 key);

    //!  \brief PrmDb PRM_SAVE_FILE command handler
    //!
    //!  This function saves the parameter values stored in RAM to the file
    //!  specified in the constructor. Any updates to parameters are not saved
    //!  until this function is called.
    //!
    //!  \param opCode The opcode of this commands
    //!  \param cmdSeq The sequence number of the command
    void PRM_SAVE_FILE_cmdHandler(FwOpcodeType opCode, U32 cmdSeq);

    //!  \brief PrmDb PRM_SAVE_FILE command handler
    //!
    //!  This function loads the parameter values from a specified
    //!  file into the backup parameter database. The command
    //!  takes a reset argument which specifies whether the existing
    //!  backup database should be cleared before loading the file, otherwise
    //!  the file contents are merged with the existing database.
    //!
    //!  \param opCode The opcode of this commands
    //!  \param cmdSeq The sequence number of the command
    //!  \param fileName The name of the parameter load file
    //!  \param merge Whether to merge (true) or fully reset (false) the parameter database from the file contents
    void PRM_LOAD_FILE_cmdHandler(FwOpcodeType opCode, U32 cmdSeq, const Fw::CmdStringArg& fileName, PrmDb_Merge merge);

    //!  \brief PrmDb PRM_COMMIT_STAGED command handler
    //!
    //! This function copies the contents of the staging parameter database
    //!  to the active parameter database, making the staged parameters
    //!  active. This command should only be called after a successful
    //!  PRM_LOAD_FILE command.
    //!
    //!  \param opCode The opcode of this commands
    //!  \param cmdSeq The sequence number of the command
    void PRM_COMMIT_STAGED_cmdHandler(FwOpcodeType opCode, U32 cmdSeq);

    //! ----------------------------------------------------------------------
    //! Helpers for database management
    //! ----------------------------------------------------------------------

    //!  \brief PrmDb clear database function
    //!
    //!  This function clears all entries from the RAM database
    //!
    //!  \param dbType The type of database to clear (active or staging)
    void clearDb(PrmDbType prmDbType);

    //!  \brief PrmDb get db pointer function
    //!  This function returns a pointer to the requested database
    //!  \param dbType The type of database requested (active or staging)
    //!  \return Pointer to the database array to be set
    t_dbStruct* getDbPtr(PrmDbType dbType);

    //!  \brief PrmDb get db string function
    //!  This function returns a string for the requested database
    //!  \param dbType The type of database requested (active or staging)
    //!  \return string representing the database
    static Fw::String getDbString(PrmDbType dbType);

    //! \brief Check param db equality
    //!
    //!  This helper method verifies the active and staging parameter dbs are equal
    bool dbEqual();

    //! \brief Deep copy for db
    //!
    //!  Copies one db to another
    //! \param dest The destination db to copy to  (active or staging)
    //! \param src The source db to copy from  (active or staging)
    void dbCopy(PrmDbType dest, PrmDbType src);

    //! \brief Deep copy for single db entry
    //!
    //!  Copies one db entry to another at specified index
    //!
    //! \param dest The destination db to copy to  (active or staging)
    //! \param src The source db to copy from  (active or staging)
    //! \param index The index of the entry to copy
    void dbCopySingle(PrmDbType dest, PrmDbType src, FwSizeType index);
};
}  // namespace Svc

#endif /* PRMDBIMPL_HPP_ */
