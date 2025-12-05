// ======================================================================
// \title  B.hpp
// \author bocchino
// \brief  hpp file for B component implementation class
// ======================================================================

#ifndef FppTest_B_HPP
#define FppTest_B_HPP

#include "Fw/Types/StringBase.hpp"
#include "FppTest/topology/components/B/BComponentAc.hpp"

namespace FppTest {

class B final : public BComponentBase {
  public:
    // ----------------------------------------------------------------------
    // Component construction and destruction
    // ----------------------------------------------------------------------

    //! Construct B object
    B(const char* const compName  //!< The component name
    );

    //! Destroy B object
    ~B();

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
