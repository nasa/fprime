// ======================================================================
// \title  Warnings.cpp
// \author bocchino
// \brief  cpp file for FileDownlink::Warnings
//
// \copyright
// Copyright 2009-2015, by the California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
// ======================================================================
#include <Svc/FileDownlink/FileDownlink.hpp>

namespace Svc {

  void FileDownlink::Warnings ::
    fileOpenError()
  {
    this->fileDownlink->log_WARNING_HI_FileOpenError(
        this->fileDownlink->file.sourceName
    );
    this->warning();
  }

  void FileDownlink::Warnings ::
    fileRead(const Os::File::Status status)
  {
    this->fileDownlink->log_WARNING_HI_FileReadError(
        this->fileDownlink->file.sourceName,
		status
    );
    this->warning();
  }

}
