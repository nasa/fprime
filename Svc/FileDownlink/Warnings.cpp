// ====================================================================== 
// \title  Warnings.cpp
// \author bocchino
// \brief  cpp file for FileDownlink::Warnings
//
// \copyright
// Copyright 2009-2015, by the California Institute of Technology.
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

#include <iostream>
#include <stdio.h>

#include <Svc/FileDownlink/FileDownlink.hpp>
#include <Fw/Types/Assert.hpp>
#include <Fw/Types/BasicTypes.hpp>

namespace Svc {

  void FileDownlink::Warnings ::
    fileOpenError(void)
  {
    this->fileDownlink->log_WARNING_HI_FileDownlink_FileOpenError(
        this->fileDownlink->file.sourceName
    );
    this->warning();
  }

  void FileDownlink::Warnings ::
    fileRead(void)
  {
    this->fileDownlink->log_WARNING_HI_FileDownlink_FileReadError(
        this->fileDownlink->file.sourceName
    );
    this->warning();
  }

}
