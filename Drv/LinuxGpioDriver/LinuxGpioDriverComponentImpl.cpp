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


#include <Drv/LinuxGpioDriver/LinuxGpioDriverComponentImpl.hpp>
#include <Fw/Types/BasicTypes.hpp>
#include <Os/TaskString.hpp>

// TODO make proper static constants for these
#define SYSFS_GPIO_DIR "/sys/class/gpio"
#define MAX_BUF 64

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <poll.h>

//#define DEBUG_PRINT(x,...) printf(x,##__VA_ARGS__); fflush(stdout)
#define DEBUG_PRINT(x,...)

namespace Drv {


// Code modified from https://developer.ridgerun.com/wiki/index.php?title=Gpio-int-test.c
    /****************************************************************
    * gpio_export
    ****************************************************************/
    int gpio_export(unsigned int gpio)
    {
        int fd, len;
        char buf[MAX_BUF];

        fd = open(SYSFS_GPIO_DIR "/export", O_WRONLY);
        if (fd < 0) {
            DEBUG_PRINT("gpio/export error!\n");
            return -1;
        }

        // TODO check value of len
        len = snprintf(buf, sizeof(buf), "%d", gpio);
        (void) write(fd, buf, len); // TODO check return value
        (void) close(fd);

	/* NOTE(mereweth) - this is to allow systemd udev to make
	 * necessary filesystem changes after exporting
	 */
	usleep(100 * 1000);
	
        return 0;
    }

    /****************************************************************
     * gpio_unexport
     ****************************************************************/
    int gpio_unexport(unsigned int gpio)
    {
        int fd, len;
        char buf[MAX_BUF];

        fd = open(SYSFS_GPIO_DIR "/unexport", O_WRONLY);
        if (fd < 0) {
            DEBUG_PRINT("gpio/unexport error!\n");
            return -1;
        }

        // TODO check value of len
        len = snprintf(buf, sizeof(buf), "%d", gpio);
        (void) write(fd, buf, len); // TODO check return value
        (void) close(fd);

	/* NOTE(mereweth) - this is to allow systemd udev to make
	 * necessary filesystem changes after unexporting
	 */
	usleep(100 * 1000);
	
        return 0;
    }

    /****************************************************************
     * gpio_set_dir
     ****************************************************************/
    int gpio_set_dir(unsigned int gpio, unsigned int out_flag)
    {
        int fd, len;
        char buf[MAX_BUF];

        len = snprintf(buf, sizeof(buf), SYSFS_GPIO_DIR  "/gpio%d/direction", gpio);
        FW_ASSERT(len > 0, len);

        fd = open(buf, O_WRONLY);
        if (fd < 0) {
            DEBUG_PRINT("gpio/direction error!\n");
            return -1;
        }

        // TODO check return value
        if (out_flag) {
            (void) write(fd, "out", 4);
        }
        else {
            (void) write(fd, "in", 3);
        }

        (void) close(fd);
        return 0;
    }

    /****************************************************************
     * gpio_set_value
     ****************************************************************/
    int gpio_set_value(int fd, unsigned int value)
    {

        FW_ASSERT(fd != -1);

        // TODO make value a enum or check its value

        // TODO check return value
        if (value) {
            (void) write(fd, "1", 1);
        }
        else {
            (void) write(fd, "0", 1);
        }

        DEBUG_PRINT("GPIO fd %d value %d written\n",fd,value);

        return 0;
    }

    /****************************************************************
     * gpio_get_value
     ****************************************************************/
    int gpio_get_value(int fd, unsigned int *value)
    {
        char ch = '0';

        FW_ASSERT(fd != -1);

        NATIVE_INT_TYPE stat1 = lseek(fd, 0, SEEK_SET); // Must seek back to the starting
        NATIVE_INT_TYPE stat2 = read(fd, &ch, 1);

        if (stat1 == -1 || stat2 != 1) {
            DEBUG_PRINT("GPIO read failure: %d %d!\n",stat1,stat2);
            return -1;
        }

        // TODO could use atoi instead to get the value
        if (ch != '0') {
            *value = 1;
        } else {
            *value = 0;
        }

        DEBUG_PRINT("GPIO fd %d value %c read\n",fd,ch);

        return 0;
    }


    /****************************************************************
     * gpio_set_edge
     ****************************************************************/

    int gpio_set_edge(unsigned int gpio, const char *edge)
    {
        int fd, len;
        char buf[MAX_BUF];

        FW_ASSERT(edge != NULL);
        // TODO check that edge has correct values of "none", "rising", or "falling"

        len = snprintf(buf, sizeof(buf), SYSFS_GPIO_DIR "/gpio%d/edge", gpio);
        FW_ASSERT(len > 0, len);

        fd = open(buf, O_WRONLY);
        if (fd < 0) {
            DEBUG_PRINT("gpio/set-edge error!\n");
            return -1;
        }

        // TODO check return value of write and strlen()
        (void) write(fd, edge, strlen(edge) + 1);
        (void) close(fd);
        return 0;
    }

    /****************************************************************
     * gpio_fd_open
     ****************************************************************/

    int gpio_fd_open(unsigned int gpio)
    {
        int fd, len;
        char buf[MAX_BUF];

        len = snprintf(buf, sizeof(buf), SYSFS_GPIO_DIR "/gpio%d/value", gpio);
        FW_ASSERT(len > 0, len);

        fd = open(buf, O_RDWR | O_NONBLOCK );
        if (fd < 0) {
            DEBUG_PRINT("gpio/fd_open error!\n");
            return -1;
        }
        return fd;
    }

    /****************************************************************
     * gpio_fd_close
     ****************************************************************/

    int gpio_fd_close(int fd, unsigned int gpio)
    {
        // TODO is this needed? w/o this the edge file and others can retain the state from
        // previous settings.
        (void) gpio_unexport(gpio); // TODO check return value

        return close(fd);
    }


  // ----------------------------------------------------------------------
  // Handler implementations for user-defined typed input ports
  // ----------------------------------------------------------------------

  void LinuxGpioDriverComponentImpl ::
    gpioRead_handler(
        const NATIVE_INT_TYPE portNum,
        bool &state
    )
  {
      FW_ASSERT(this->m_fd != -1);

      NATIVE_UINT_TYPE val;
      NATIVE_INT_TYPE stat = gpio_get_value(this->m_fd, &val);
      if (-1 == stat) {
          this->log_WARNING_HI_GP_ReadError(this->m_gpio,stat);
          return;
      } else {
          state = val?true:false;
      }

  }

  void LinuxGpioDriverComponentImpl ::
    gpioWrite_handler(
        const NATIVE_INT_TYPE portNum,
        bool state
    )
  {
      FW_ASSERT(this->m_fd != -1);

      NATIVE_INT_TYPE stat;

      stat = gpio_set_value(this->m_fd,state?1:0);

      if (0 != stat) {
          this->log_WARNING_HI_GP_WriteError(this->m_gpio,stat);
          return;
      }
  }

  bool LinuxGpioDriverComponentImpl ::
    open(NATIVE_INT_TYPE gpio, GpioDirection direction) {

      // TODO check for invalid gpio?
      NATIVE_INT_TYPE stat;

      // Configure:
      stat = gpio_export(gpio);
      if (-1 == stat) {
          Fw::LogStringArg arg = strerror(errno);
          this->log_WARNING_HI_GP_OpenError(gpio,stat,arg);
          return false;
      }
      stat = gpio_set_dir(gpio, direction == GPIO_OUT ? 1 : 0);
      if (-1 == stat) {
          Fw::LogStringArg arg = strerror(errno);
          this->log_WARNING_HI_GP_OpenError(gpio,stat,arg);
          return false;
      }

      // If needed, set edge to rising in intTaskEntry()

      // Open:
      this->m_fd = gpio_fd_open(gpio);
      if (-1 == this->m_fd) {
          Fw::LogStringArg arg = strerror(errno);
          this->log_WARNING_HI_GP_OpenError(gpio,errno,arg);
      } else {
          this->m_gpio = gpio;
      }


      return true;
  }

  //! Entry point for task waiting for RTI
  void LinuxGpioDriverComponentImpl ::
    intTaskEntry(void * ptr) {

      FW_ASSERT(ptr);
      LinuxGpioDriverComponentImpl* compPtr = (LinuxGpioDriverComponentImpl*) ptr;

      FW_ASSERT(compPtr->m_fd != -1);

      // start GPIO interrupt
      NATIVE_INT_TYPE stat;
      stat = gpio_set_edge(compPtr->m_gpio, "rising");
      if (-1 == stat) {
          compPtr->log_WARNING_HI_GP_IntStartError(compPtr->m_gpio);
          return;
      }

      // spin waiting for interrupt
      while(not compPtr->m_quitThread) {
          pollfd fdset[1];
          NATIVE_INT_TYPE nfds = 1;
          NATIVE_INT_TYPE timeout = 10000; // Timeout of 10 seconds

          memset((void*)fdset, 0, sizeof(fdset));

          fdset[0].fd = compPtr->m_fd;
          fdset[0].events = POLLPRI;

          stat = poll(fdset, nfds, timeout);

          /*
           * According to this link, poll will always have POLLERR set for the sys/class/gpio subsystem
           * so cant check for it to look for error:
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

              char *buf[MAX_BUF];
              (void) lseek(fdset[0].fd, 0, SEEK_SET); // Must seek back to the starting
              (void) read(fdset[0].fd, buf, MAX_BUF);
              DEBUG_PRINT("\npoll() GPIO interrupt occurred w/ value: %c\n", buf[0]);
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

  Os::Task::TaskStatus LinuxGpioDriverComponentImpl ::
  startIntTask(NATIVE_INT_TYPE priority, NATIVE_INT_TYPE cpuAffinity) {
      Os::TaskString name;
      name.format("GPINT_%s",this->getObjName()); // The task name can only be 16 chars including null
      Os::Task::TaskStatus stat = this->m_intTask.start(name,0,priority,20*1024,LinuxGpioDriverComponentImpl::intTaskEntry,this,cpuAffinity);

      if (stat != Os::Task::TASK_OK) {
          DEBUG_PRINT("Task start error: %d\n",stat);
      }

      return stat;

  }

  void LinuxGpioDriverComponentImpl ::
    exitThread(void) {
      this->m_quitThread = true;
  }



  LinuxGpioDriverComponentImpl ::
    ~LinuxGpioDriverComponentImpl(void)
  {
      if (this->m_fd != -1) {
          DEBUG_PRINT("Closing GPIO %d fd %d\n",this->m_gpio, this->m_fd);
          (void) gpio_fd_close(this->m_fd, this->m_gpio);
      }

  }


} // end namespace Drv
