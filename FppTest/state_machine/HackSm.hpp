
// ======================================================================
// \title  HackSm.h
// \author Auto-generated
// \brief  header file for state machine HackSm
//
// ======================================================================
           
#ifndef HACKSM_H_
#define HACKSM_H_
                                
#include <Fw/SMSignal/SMSignalBuffer.hpp>
#include <config/FpConfig.hpp>
                                 
namespace Fw {
  class SMSignals;
}

namespace FppTest {

class HackSm_Interface {
  public:
    enum HackSmEvents {
      RTI_SIG,
      CHECK_SIG,
    };

                                 
    virtual void HackSm_turnOff(const FwEnumStoreType stateMachineId) = 0;
                                 
                                 
    virtual void HackSm_turnOn(const FwEnumStoreType stateMachineId) = 0;
                                 
                                 
    virtual void HackSm_doDiag(const FwEnumStoreType stateMachineId) = 0;
                                 
                                                                  
};

class HackSm {
                                 
  private:
    HackSm_Interface *parent;
                                 
  public:
                                 
    HackSm(HackSm_Interface* parent) : parent(parent) {}
  
    enum HackSmStates {
      OFF,
      ON,
      DIAG,
    };
    
    enum HackSmStates state;

    void * extension;

    void init(const FwEnumStoreType stateMachineId);
    void update(
        const FwEnumStoreType stateMachineId, 
        const HackSm_Interface::HackSmEvents signal, 
        const Fw::SMSignalBuffer &data
    );
};

}

#endif
