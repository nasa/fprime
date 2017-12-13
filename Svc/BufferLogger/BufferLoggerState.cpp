// ====================================================================== 
// \title  State.cpp
// \author bocchino, dinkel
// \brief  Implementation for ASTERIA::BufferLogger::State
//
// \copyright
// Copyright (C) 2015-2017 California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged. Any commercial use must be negotiated with the Office
// of Technology Transfer at the California Institute of Technology.
// 
// This software may be subject to U.S. export control laws and
// regulations.  By accepting this document, the user agrees to comply
// with all U.S. export laws and regulations.  User has the
// responsibility to obtain export licenses, or other export authority
// as may be required before exporting such information to foreign
// countries or providing access to foreign persons.
// ====================================================================== 

#include "ASTERIA/Components/BufferLogger/BufferLogger.hpp"

namespace ASTERIA {

  BufferLogger::State ::
    State(
        BufferLogger& bufferLogger,
        const char *const fileName
    ) :
      bufferLogger(bufferLogger),
      volatileState(OnOff::ON),
      nonVolatileState(sizeof(U8), 1, fileName)
  {

  }

  OnOff::t BufferLogger::State ::
    get(void) const
  {
    return this->volatileState;
  }

  void BufferLogger::State ::
    set(const OnOff::t state)
  {
    this->volatileState = state;
    this->bufferLogger.log_ACTIVITY_LO_SetState(state);
  }

  NonVolatileU8::Status::t BufferLogger::State ::
    save(void)
  {
    U8 value;
    switch (this->volatileState) {
      case OnOff::OFF:
        value = 0;
        break;
      case OnOff::ON:
        value = 1;
        break;
      default:
        FW_ASSERT(0, this->volatileState);
        break;
    }
    const NonVolatileU8::Status::t status =
      this->nonVolatileState.write(value);
    if (status != NonVolatileU8::Status::SUCCESS) {
      this->bufferLogger.log_WARNING_HI_NonVolatileWriteError();
    }
    return status;
  }

  NonVolatileU8::Status::t BufferLogger::State ::
    load(void)
  {
    U8 value;
    const NonVolatileU8::Status::t status =
      this->nonVolatileState.read(value);
    if (status == NonVolatileU8::Status::SUCCESS) {
      switch (value) {
        case 0:
          this->volatileState = OnOff::OFF;
          break;
        case 1:
          this->volatileState = OnOff::ON;
          break;
        default:
          FW_ASSERT(0, value);
          break;
      }
      this->bufferLogger.log_ACTIVITY_LO_LoadState(this->volatileState);
    }
    else {
      this->bufferLogger.log_WARNING_HI_NonVolatileReadError();
    }
    return status;
  }

}
