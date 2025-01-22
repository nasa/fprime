
// ======================================================================
// \title  DeviceSm.h
// \author Auto-generated
// \brief  header file for state machine DeviceSm
//
// ======================================================================
           
#ifndef DEVICESM_H_
#define DEVICESM_H_
                                
#include <Fw/Sm/SmSignalBuffer.hpp>
#include <config/FpConfig.hpp>
                                 
namespace FppTest {

class DeviceSm_Interface {
  public:
    enum DeviceSm_Signals {
      RTI_SIG,
    };

                                 
    virtual bool DeviceSm_g1(const FwEnumStoreType stateMachineId) = 0;
                                 
                                 
    virtual bool DeviceSm_g2(
        const FwEnumStoreType stateMachineId, 
        const DeviceSm_Interface::DeviceSm_Signals signal, 
        const Fw::SmSignalBuffer &data) = 0;
                                 
                                 
    virtual void DeviceSm_turnOff(const FwEnumStoreType stateMachineId) = 0;
                                 
                                 
    virtual  void DeviceSm_a1(
        const FwEnumStoreType stateMachineId, 
        const DeviceSm_Interface::DeviceSm_Signals signal, 
        const Fw::SmSignalBuffer &data) = 0;
                                 
                                 
    virtual void DeviceSm_turnOn(const FwEnumStoreType stateMachineId) = 0;
                                 
                                 
    virtual void DeviceSm_a2(const FwEnumStoreType stateMachineId) = 0;
                                 
                                                                  
};

class DeviceSm {
                                 
  private:
    DeviceSm_Interface *parent;
                                 
  public:
                                 
    DeviceSm(DeviceSm_Interface* parent) : parent(parent) {}
  
    enum DeviceSm_States {
      OFF,
      ON,
    };
    
    enum DeviceSm_States state;

    void init(const FwEnumStoreType stateMachineId);
    void update(
        const FwEnumStoreType stateMachineId, 
        const DeviceSm_Interface::DeviceSm_Signals signal, 
        const Fw::SmSignalBuffer &data
    );
};

}

#endif
