// ======================================================================
// \title  Sender.hpp
// \author bocchino
// \brief  hpp file for Sender component implementation class
// ======================================================================

#ifndef FppTest_A_HPP
#define FppTest_A_HPP

#include "Fw/Types/StringBase.hpp"
#include "FppTest/topology/components/Sender/SenderComponentAc.hpp"

namespace FppTest {

class Sender final : public AComponentBase {
  public:
    // ----------------------------------------------------------------------
    // Component construction and destruction
    // ----------------------------------------------------------------------

    //! Construct A object
    Sender(const char* const compName  //!< The component name
    );

    //! Destroy A object
    ~Sender();

  private:
    // ----------------------------------------------------------------------
    // Handler implementations for typed input ports
    // ----------------------------------------------------------------------

    //! Handler implementation for dataIn
    void dataIn_handler(FwIndexType portNum,  //!< The port number
                        U32 data              //!< The data
                        ) override;

  public:
    // ----------------------------------------------------------------------
    // Public interface
    // ----------------------------------------------------------------------

    //! Send data
    void sendData(U32 data  //!< The data
    );

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
