// ======================================================================
// \title  CfdpManager.hpp
// \author campuzan
// \brief  hpp file for CfdpManager component implementation class
// ======================================================================

#ifndef Ccsds_CfdpManager_HPP
#define Ccsds_CfdpManager_HPP

#include "Svc/Ccsds/CfdpManager/CfdpManagerComponentAc.hpp"

namespace Svc {
namespace Ccsds {

class CfdpManager final : public CfdpManagerComponentBase {
  public:
    // ----------------------------------------------------------------------
    // Component construction and destruction
    // ----------------------------------------------------------------------

    //! Construct CfdpManager object
    CfdpManager(const char* const compName  //!< The component name
    );

    //! Destroy CfdpManager object
    ~CfdpManager();

  private:
    // ----------------------------------------------------------------------
    // Handler implementations for typed input ports
    // ----------------------------------------------------------------------

    //! Handler implementation for run1Hz
    //!
    //! Run port which must be invoked at 1 Hz in order to satify CFDP timer logic
    void run1Hz_handler(FwIndexType portNum,  //!< The port number
                        U32 context           //!< The call order
                        ) override;
                        
  private:
    // ----------------------------------------------------------------------
    // Handler implementations for commands
    // ----------------------------------------------------------------------

    //! Handler implementation for command TODO
    //!
    //! TODO
    void TODO_cmdHandler(FwOpcodeType opCode,  //!< The opcode
                         U32 cmdSeq            //!< The command sequence number
                         ) override;
};

}  // namespace Ccsds
}  // namespace Svc

#endif
