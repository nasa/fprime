// ======================================================================
// \title  SmTest.hpp
// \author watney
// \brief  hpp file for SmTest component implementation class
// ======================================================================

#ifndef FppTest_SmTest_HPP
#define FppTest_SmTest_HPP

#include <array>

#include "FppTest/state_machine/external_instance/SmTestComponentAc.hpp"
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

    //! Overflow hook for state machine device4
    void device4_stateMachineOverflowHook(
        const HackSm_Interface::HackSm_Signals signal, //!< The state machine signal
        const Fw::SmSignalBuffer& data //!< The state machine data
    );

    // State machine functions
    void DeviceSm_turnOn(const FwEnumStoreType stateMachineId);
    
    void DeviceSm_turnOff(const FwEnumStoreType stateMachineId);

    void DeviceSm_a1(
      const FwEnumStoreType stateMachineId, 
      const DeviceSm_Signals signal, 
      const Fw::SmSignalBuffer& data
    );
    
    void DeviceSm_a2(const FwEnumStoreType stateMachineId);
    
    bool DeviceSm_g1(const FwEnumStoreType stateMachineId);
    
    bool DeviceSm_g2(
      const FwEnumStoreType stateMachineId, 
      const DeviceSm_Signals signal, 
      const Fw::SmSignalBuffer& data
    );

    void HackSm_turnOn(const FwEnumStoreType stateMachineId);
    
    void HackSm_turnOff(const FwEnumStoreType stateMachineId);

    void HackSm_doDiag(const FwEnumStoreType stateMachineId);

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
