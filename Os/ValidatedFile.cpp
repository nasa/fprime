// ======================================================================
// \title  ValidatedFile.cpp
// \author bocchino
// \brief  Os::ValidatedFile implementation
//
// \copyright
// Copyright (C) 2017 California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
//
// ======================================================================

#include "Os/ValidatedFile.hpp"
#include "Fw/Types/Assert.hpp"
#include "Utils/Hash/Hash.hpp"

namespace Os {

ValidatedFile ::ValidatedFile(const char* const fileName) : m_fileName(fileName), m_hashFileName(""), m_hashBuffer() {
    const Fw::FormatStatus formatStatus = Utils::Hash::addFileExtension(this->m_fileName, this->m_hashFileName);
    // A truncated hash file name would silently validate against the wrong file
    FW_ASSERT(formatStatus == Fw::FormatStatus::SUCCESS, static_cast<FwAssertArgType>(formatStatus));
}

Os::ValidateFile::Status ValidatedFile ::validate() {
    const Os::ValidateFile::Status status =
        Os::ValidateFile::validate(this->m_fileName.toChar(), this->m_hashFileName.toChar(), this->m_hashBuffer);
    return status;
}

Os::ValidateFile::Status ValidatedFile ::createHashFile() {
    const Os::ValidateFile::Status status = Os::ValidateFile::createValidation(
        this->m_fileName.toChar(), this->m_hashFileName.toChar(), this->m_hashBuffer);
    return status;
}

Os::ValidateFile::Status ValidatedFile ::createHashFile(const Utils::HashBuffer& hashBuffer) {
    this->m_hashBuffer = hashBuffer;
    const Os::ValidateFile::Status status =
        Os::ValidateFile::createValidation(this->m_hashFileName.toChar(), this->m_hashBuffer);
    return status;
}

const Fw::ConstStringBase& ValidatedFile ::getFileName() const {
    return this->m_fileName;
}

const Fw::ConstStringBase& ValidatedFile ::getHashFileName() const {
    return this->m_hashFileName;
}

const Utils::HashBuffer& ValidatedFile ::getHashBuffer() const {
    return this->m_hashBuffer;
}

}  // namespace Os
