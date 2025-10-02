module Drv {

  @ A passive buffer driver
  interface PassiveBufferDriver {

    @ The interface for sending data to the driver
    import Fw.PassiveBufferIn

    @ The interface for receiving data from the driver
    import Fw.PassiveBufferOut

  }

}
