module Svc {

  @ A Linux interval timer
  passive component LinuxTimer {

    @ implement tick interface
    include "../../Drv/Interfaces/TickInterface.fppi"

  }

}
