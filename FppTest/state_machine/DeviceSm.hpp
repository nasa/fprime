
// ======================================================================
// \title  DeviceSm.h
// \author Auto-generated
// \brief  header file for state machine DeviceSm
//
// ======================================================================
           
#ifndef DEVICESM_H_
#define DEVICESM_H_
                                
#include <Fw/SMSignal/SMSignalBuffer.hpp>
#include <config/FpConfig.hpp>
                                 
namespace Fw {
  class SMSignals;
}

namespace FppTest {

class DeviceSm_Interface {
  public:
    enum DeviceSmEvents {
      RTI_SIG,
    };

                                 
    virtual bool DeviceSm_g1(const FwEnumStoreType stateMachineId) = 0;
                                 
                                 
    virtual bool DeviceSm_g2(
        const FwEnumStoreType stateMachineId, 
        const DeviceSm_Interface::DeviceSmEvents signal, 
        const Fw::SMSignalBuffer &data) = 0;
                                 
                                 
    virtual void DeviceSm_turnOff(const FwEnumStoreType stateMachineId) = 0;
                                 
                                 
    virtual  void DeviceSm_a1(
        const FwEnumStoreType stateMachineId, 
        const DeviceSm_Interface::DeviceSmEvents signal, 
        const Fw::SMSignalBuffer &data) = 0;
                                 
                                 
    virtual void DeviceSm_turnOn(const FwEnumStoreType stateMachineId) = 0;
                                 
                                 
    virtual void DeviceSm_a2(const FwEnumStoreType stateMachineId) = 0;
                                 
                                                                  
};

class DeviceSm {
                                 
  private:
    DeviceSm_Interface *parent;
                                 
  public:
                                 
    DeviceSm(DeviceSm_Interface* parent) : parent(parent) {}
  
    enum DeviceSmStates {
      OFF,
      ON,
    };
    
    enum DeviceSmStates state;

    void * extension;

    void init(const FwEnumStoreType stateMachineId);
    void update(
        const FwEnumStoreType stateMachineId, 
        const DeviceSm_Interface::DeviceSmEvents signal, 
        const Fw::SMSignalBuffer &data
    );
};

}

#endif
