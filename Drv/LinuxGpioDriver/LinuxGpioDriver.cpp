// ======================================================================
// \title  LinuxGpioDriverImpl.cpp
// \author tcanham
// \brief  cpp file for LinuxGpioDriver component implementation class
//
// \copyright
// Copyright 2009-2015, by the California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
//
// ======================================================================
#include <Os/Posix/File.hpp>

#include <Drv/LinuxGpioDriver/LinuxGpioDriver.hpp>
#include <FpConfig.hpp>
#include <Os/TaskString.hpp>
#include <cerrno>


// TODO make proper static constants for these
#define SYSFS_GPIO_DIR "/sys/class/gpio"
#define MAX_BUF 64

#include <cstdio>
#include <cstdlib>
#include <cstring>

#include <unistd.h>
#include <fcntl.h>
#include <poll.h>


namespace Drv {

#ifdef V2
GPIO_V2_GET_LINE_IOCTL, struct gpio_v2_line_request *request
#else

#endif

Os::File::Status errno_to_status(PlatformIntType errno_input) {
    Os::File::Status status = Os::File::Status::OTHER_ERROR;
    switch (errno_input) {
        case EBADF:
            status = Os::File::Status::NOT_OPENED;
            break;
        case EFAULT:
            break;
        case  EINVAL:
            status = Os::File::Status::INVALID_ARGUMENT;
            break;
        case ENODEV:
            status = Os::File::Status::DOESNT_EXIST;
            break;
        case ENOMEM:
            status = Os::File::Status::NO_SPACE;
            break;
        case EPERM:
            status = Os::File::Status::NO_PERMISSION;
            break;
        case ENXIO:
            status = Os::File::Status::INVALID_MODE;
            break;
        // Cascades intended
        case EWOULDBLOCK:
        case EBUSY:
        case EIO:
        default:
            status = Os::File::Status::OTHER_ERROR;
            break;
    }
    return status;
}


  // ----------------------------------------------------------------------
  // Handler implementations for user-defined typed input ports
  // ----------------------------------------------------------------------

Os::File::Status LinuxGpioDriver ::open(const char* chip, FwSizeType gpio, GpioConfiguration direction) {
    Os::File::Status status = Os::File::OP_OK;
    Os::File chip_file; // TODO: how to force posix file safely

    // Open chip file and check for success
    status = chip_file.open(chip, Os::File::Mode::OPEN_WRITE);
    if (status == Os::File::OP_OK) {
        struct gpiochip_info chip_info;
        PlatformIntType fd = reinterpret_cast<Os::Posix::File::PosixFileHandle*>(chip_file.getHandle())->m_file_descriptor;
        PlatformIntType return_value = ioctl(fd, GPIO_GET_CHIPINFO_IOCTL, chip_info);
        if (return_value == 0) {
            this->log_ACTIVITY_HI_OpenChip(
                Fw::String(chip_info.name), Fw::String(chip_info.label), chip_info.lines);
            Os::File pin_file; // TODO: how to force posix file safely
            int ioctl(int chip_fd, GPIO_GET_LINEHANDLE_IOCTL, struct )
        }





    } else {
        this->log_WARNING_HI_OpenChipError(
            Fw::String(chip), Os::FileStatus(static_cast<Os::FileStatus::T>(status)));
    }



    return status;
}

  void LinuxGpioDriver ::
    gpioRead_handler(
        const NATIVE_INT_TYPE portNum,
        Fw::Logic &state
    )
  {
      FW_ASSERT(this->m_fd != -1);

      NATIVE_UINT_TYPE val;
      NATIVE_INT_TYPE stat = gpio_get_value(this->m_fd, &val);
      if (-1 == stat) {
          this->log_WARNING_HI_GP_ReadError(this->m_gpio,stat);
          return;
      } else {
          state = val ? Fw::Logic::HIGH : Fw::Logic::LOW;
      }

  }

  void LinuxGpioDriver ::
    gpioWrite_handler(
        const NATIVE_INT_TYPE portNum,
        const Fw::Logic& state
    )
  {
      FW_ASSERT(this->m_fd != -1);

      NATIVE_INT_TYPE stat;

      stat = gpio_set_value(this->m_fd,(state == Fw::Logic::HIGH) ? 1 : 0);

      if (0 != stat) {
          this->log_WARNING_HI_GP_WriteError(this->m_gpio,stat);
          return;
      }
  }

  

  //! Entry point for task waiting for RTI
  void LinuxGpioDriver ::
    intTaskEntry(void * ptr) {

    FW_ASSERT(ptr);
    LinuxGpioDriver* compPtr = static_cast<LinuxGpioDriver*>(ptr);
    FW_ASSERT(compPtr->m_fd != -1);

    // start GPIO interrupt
    NATIVE_INT_TYPE stat;
    stat = gpio_set_edge(static_cast<unsigned int>(compPtr->m_gpio), "rising");
    if (-1 == stat) {
      compPtr->log_WARNING_HI_GP_IntStartError(compPtr->m_gpio);
      return;
    }

    // spin waiting for interrupt
    while(not compPtr->m_quitThread) {
        pollfd fdset[1];
        NATIVE_INT_TYPE nfds = 1;
        NATIVE_INT_TYPE timeout = 10000; // Timeout of 10 seconds

        memset(fdset, 0, sizeof(fdset));

        fdset[0].fd = compPtr->m_fd;
        fdset[0].events = POLLPRI;
        stat = poll(fdset, static_cast<nfds_t>(nfds), timeout);

        /*
        * According to this link, poll will always have POLLERR set for the sys/class/gpio subsystem
        * so can't check for it to look for error:
        * http://stackoverflow.com/questions/27411013/poll-returns-both-pollpri-pollerr
        */
        if (stat < 0) {
            DEBUG_PRINT("stat: %d, revents: 0x%x, POLLERR: 0x%x, POLLIN: 0x%x, POLLPRI: 0x%x\n",
                    stat, fdset[0].revents, POLLERR, POLLIN, POLLPRI); // TODO remove
            compPtr->log_WARNING_HI_GP_IntWaitError(compPtr->m_gpio);
            return;
        }

        if (stat == 0) {
            // continue to poll
            DEBUG_PRINT("Krait timed out waiting for GPIO interrupt\n");
            continue;
        }

        // Asserting that number of fds w/ revents is 1:
        FW_ASSERT(stat == 1, stat);  // TODO should i bother w/ this assert?

        // TODO what to do if POLLPRI not set?

        // TODO: if I take out the read then the poll just continually interrupts
        // Read is only taking 22 usecs each time, so it is not blocking for long
        if (fdset[0].revents & POLLPRI) {

            char buf[MAX_BUF];
            (void) lseek(fdset[0].fd, 0, SEEK_SET); // Must seek back to the starting
            if(read(fdset[0].fd, buf, MAX_BUF) > 0) {
                DEBUG_PRINT("\npoll() GPIO interrupt occurred w/ value: %c\n", buf[0]);
            }
        }

        // call interrupt ports
        Svc::TimerVal timerVal;
        timerVal.take();

        for (NATIVE_INT_TYPE port = 0; port < compPtr->getNum_intOut_OutputPorts(); port++) {
            if (compPtr->isConnected_intOut_OutputPort(port)) {
                compPtr->intOut_out(port,timerVal);
            }
        }

      }

  }

  Os::Task::Status LinuxGpioDriver ::
  startIntTask(Os::Task::ParamType priority, Os::Task::ParamType stackSize, Os::Task::ParamType cpuAffinity) {
      Os::TaskString name;
      name.format("GPINT_%s",this->getObjName()); // The task name can only be 16 chars including null
      Os::Task::Arguments arguments(name, LinuxGpioDriver::intTaskEntry, this, priority, stackSize, cpuAffinity);
      Os::Task::Status stat = this->m_intTask.start(arguments);

      if (stat != Os::Task::OP_OK) {
          DEBUG_PRINT("Task start error: %d\n",stat);
      }

      return stat;

  }

  void LinuxGpioDriver ::
    exitThread() {
      this->m_quitThread = true;
  }



  LinuxGpioDriver ::
    ~LinuxGpioDriver()
  {
      if (this->m_fd != -1) {
          DEBUG_PRINT("Closing GPIO %d fd %d\n",this->m_gpio, this->m_fd);
          (void) gpio_fd_close(this->m_fd, static_cast<unsigned int>(this->m_gpio));
      }

  }


} // end namespace Drv
