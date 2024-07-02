
// ======================================================================
// \title  DeviceSm.h
// \author Auto-generated
// \brief  header file for state machine DeviceSm
//
// ======================================================================
           
#ifndef DEVICESM_H_
#define DEVICESM_H_

namespace Fw {
  class SMSignals;
}

namespace FppTest {

class DeviceSm_Interface {
  public:
                                                                  
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

    enum DeviceSmEvents {
      RTI_SIG,
    };
    
    enum DeviceSmStates state;

    void * extension;

    void init();
    void update(const Fw::SMSignals *e);

};

}

#endif
