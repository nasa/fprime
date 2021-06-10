/*
 * LinuxSerialDriverComponentImplCommon.cpp
 *
 *  Created on: Nov 29, 2016
 *      Author: tcanham
 */

#include <Drv/LinuxSerialDriver/LinuxSerialDriverComponentImpl.hpp>

namespace Drv {


  // ----------------------------------------------------------------------
  // Construction, initialization, and destruction
  // ----------------------------------------------------------------------

    LinuxSerialDriverComponentImpl ::
      LinuxSerialDriverComponentImpl(
          const char *const compName
      ) : LinuxSerialDriverComponentBase(compName),
          m_fd(-1),
          m_device("NOT_EXIST"),
          m_quitReadThread(false)
    {
        // initialize buffer set
        for (NATIVE_INT_TYPE entry = 0; entry < DR_MAX_NUM_BUFFERS; entry++) {
            this->m_buffSet[entry].available = false;
        }

    }

    void LinuxSerialDriverComponentImpl ::
      init(
          const NATIVE_INT_TYPE instance
      )
    {
      LinuxSerialDriverComponentBase::init(instance);
    }

    void LinuxSerialDriverComponentImpl ::
      readBufferSend_handler(
          const NATIVE_INT_TYPE portNum,
          Fw::Buffer& Buffer
      )
    {
        this->m_readBuffMutex.lock();
        bool found = false;

        // search for open entry
        for (NATIVE_UINT_TYPE entry = 0; entry < DR_MAX_NUM_BUFFERS; entry++) {
            // Look for slots to fill. "Available" is from
            // the perspective of the driver thread looking for
            // buffers to fill, so add the buffer and make it available.
            if (not this->m_buffSet[entry].available) {
                this->m_buffSet[entry].readBuffer = Buffer;
                this->m_buffSet[entry].available = true;
                found = true;
                break;
            }
        }
        this->m_readBuffMutex.unLock();
        FW_ASSERT(found,Buffer.getContext());

    }

}
