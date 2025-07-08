// ======================================================================
// \title  ActivePhaser.hpp
// \author shaokail
// \brief  hpp file for ActivePhaser component implementation class
// ======================================================================

#ifndef Svc_ActivePhaser_HPP
#define Svc_ActivePhaser_HPP

#include "Svc/ActivePhaser/ActivePhaserComponentAc.hpp"

namespace Svc {

class ActivePhaser final : public ActivePhaserComponentBase {
  public:
    // ----------------------------------------------------------------------
    // Component construction and destruction
    // ----------------------------------------------------------------------

    //! Construct ActivePhaser object
    ActivePhaser(const char* const compName  //!< The component name
    );

    //! Destroy ActivePhaser object
    ~ActivePhaser();

  PRIVATE:
    // ----------------------------------------------------------------------
    // Handler implementations for typed input ports
    // ----------------------------------------------------------------------

    //! Handler implementation for CycleIn
    void CycleIn_handler(FwIndexType portNum,     //!< The port number
                         Os::RawTime& cycleStart  //!< Cycle start timestamp
                         ) override;

  PRIVATE:
    // ----------------------------------------------------------------------
    // Handler implementations for user-defined internal interfaces
    // ----------------------------------------------------------------------

    //! Handler implementation for Tick
    //!
    //! An internal port for sending data of type T
    void Tick_internalInterfaceHandler() override;
};

}  // namespace Svc

#endif
