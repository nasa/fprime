
// ======================================================================
// \title  DeviceSm.h
// \author Auto-generated
// \brief  header file for state machine DeviceSm
//
// ======================================================================
           
#ifndef DEVICESM_H_
#define DEVICESM_H_

namespace Fw {
  class SMEvents;
}

namespace FppTest {

class DeviceSmIf {
  public:
                                                                  
};

class DeviceSm {
                                 
  private:
    DeviceSmIf *parent;
                                 
  public:
                                 
    DeviceSm(DeviceSmIf* parent) : parent(parent) {}
  
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
    void update(const Fw::SMEvents *e);

};

}

#endif
