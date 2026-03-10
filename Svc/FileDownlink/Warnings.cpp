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

void FileDownlink::Warnings ::fileOpenError() {
    this->m_fileDownlink->log_WARNING_HI_FileOpenError(this->m_fileDownlink->m_file.getSourceName());
    this->warning();
}

void FileDownlink::Warnings ::fileRead(const Os::File::Status status) {
    this->m_fileDownlink->log_WARNING_HI_FileReadError(this->m_fileDownlink->m_file.getSourceName(), status);
    this->warning();
}

void FileDownlink::Warnings ::zeroSize() {
    this->m_fileDownlink->log_WARNING_HI_DownlinkZeroSizeFile(this->m_fileDownlink->m_file.getSourceName());
    this->warning();
}

}  // namespace Svc
