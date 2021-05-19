// ====================================================================== 
// \title  Warnings.cpp
// \author bocchino
// \brief  cpp file for FileDownlink::Warnings
//
// \copyright
// Copyright 2009-2015, by the California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
// 
// ====================================================================== 

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
