 // ======================================================================
 // \title  LinuxUartDriver.hpp
 // \brief  DEPRECATED: Use PosixUartDriver instead
 // ======================================================================
 #ifndef LinuxUartDriver_HPP
 #define LinuxUartDriver_HPP

 #include <Drv/PosixUartDriver/PosixUartDriver.hpp>

 namespace Drv {
 // Deprecated alias for backward compatibility
 using LinuxUartDriver = PosixUartDriver;
 }  // namespace Drv

 #endif
