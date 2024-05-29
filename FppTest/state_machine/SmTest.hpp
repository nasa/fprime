// ======================================================================
// \title  SmTest.hpp
// \author watney
// \brief  hpp file for SmTest component implementation class
// ======================================================================

#ifndef FppTest_SmTest_HPP
#define FppTest_SmTest_HPP

#include <array>

#include "FppTest/state_machine/SmTestComponentAc.hpp"
#include "Fw/Types/String.hpp"

namespace FppTest {

class SmTest : 
  public SmTestComponentBase 
{

    // Friend class for testing
    friend class Tester;

  public:
    // ----------------------------------------------------------------------
    // Constants
    // ----------------------------------------------------------------------

 

  public:
    // ----------------------------------------------------------------------
    // Types
    // ----------------------------------------------------------------------

  
  public:
    // ----------------------------------------------------------------------
    // Construction, initialization, and destruction
    // ----------------------------------------------------------------------

    //! Construct object SmTest
    SmTest(const char* const compName);                     //!< The component name

    //! Initialize object SmTest
    void init(const NATIVE_INT_TYPE queueDepth,   //!< The queue depth
              const NATIVE_INT_TYPE instance = 0  //!< The instance number
    );

    //! Destroy object SmTest
    ~SmTest();

  public:
    // ----------------------------------------------------------------------
    // Public interface methods
    // ----------------------------------------------------------------------

 

  private:
    // ----------------------------------------------------------------------
    // Handler implementations for user-defined typed input ports
    // ----------------------------------------------------------------------

    //! Handler implementation for schedIn
    void schedIn_handler(const NATIVE_INT_TYPE portNum,  //!< The port number
                         U32 context                     //!< The call order
                         ) final;

  private:
    // ----------------------------------------------------------------------
    // Data product handler implementations
    // ----------------------------------------------------------------------

  
  private:
    // ----------------------------------------------------------------------
    // Private helper functions
    // ----------------------------------------------------------------------

  

  private:
    // ----------------------------------------------------------------------
    // Private member variables
    // ----------------------------------------------------------------------

    
};

}  // end namespace FppTest

#endif
