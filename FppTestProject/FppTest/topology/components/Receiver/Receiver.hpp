// ======================================================================
// \title  Receiver.hpp
// \author bocchino
// \brief  hpp file for Receiver component implementation class
// ======================================================================

#ifndef FppTest_B_HPP
#define FppTest_B_HPP

#include "Fw/Types/StringBase.hpp"
#include "FppTest/topology/components/Receiver/ReceiverComponentAc.hpp"

namespace FppTest {

class Receiver final : public ReceiverComponentBase {
  public:
    // ----------------------------------------------------------------------
    // Component construction and destruction
    // ----------------------------------------------------------------------

    //! Construct B object
    Receiver(const char* const compName  //!< The component name
    );

    //! Destroy B object
    ~Receiver();

  private:
    // ----------------------------------------------------------------------
    // Handler implementations for typed input ports
    // ----------------------------------------------------------------------

    //! Handler implementation for dataIn
    void dataIn_handler(FwIndexType portNum,  //!< The port number
                        U32 data) override;

  private:
    // ----------------------------------------------------------------------
    // Private helper methods
    // ----------------------------------------------------------------------

    //! Print a message
    void printMessage(const Fw::StringBase& msg  //!< The message
    );
};

}  // namespace FppTest

#endif
