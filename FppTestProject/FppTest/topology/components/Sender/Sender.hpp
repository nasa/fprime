// ======================================================================
// \title  Sender.hpp
// \author tumbar
// \brief  hpp file for Sender component implementation class
// ======================================================================

#ifndef FppTest_Sender_HPP
#define FppTest_Sender_HPP

#include "FppTest/topology/components/Sender/SenderComponentAc.hpp"

namespace FppTest {

class Sender final : public SenderComponentBase {
  public:
    // ----------------------------------------------------------------------
    // Component construction and destruction
    // ----------------------------------------------------------------------

    //! Construct Sender object
    Sender(const char* const compName  //!< The component name
    );

    //! Destroy Sender object
    ~Sender();
};

}  // namespace FppTest

#endif
