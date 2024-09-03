
// ======================================================================
// \title  HackSm.h
// \author Auto-generated
// \brief  header file for state machine HackSm
//
// ======================================================================
           
#ifndef HACKSM_H_
#define HACKSM_H_
                                
#include <Fw/Sm/SmSignalBuffer.hpp>
#include <config/FpConfig.hpp>
                                 
namespace FppTest {

class HackSm_Interface {
  public:
    enum HackSm_Signals {
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
  
    enum HackSm_States {
      OFF,
      ON,
      DIAG,
    };
    
    enum HackSm_States state;

    void init(const FwEnumStoreType stateMachineId);
    void update(
        const FwEnumStoreType stateMachineId, 
        const HackSm_Interface::HackSm_Signals signal, 
        const Fw::SmSignalBuffer &data
    );
};

}

#endif
