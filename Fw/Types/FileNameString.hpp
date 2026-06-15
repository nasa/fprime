// ======================================================================
// @file   FileNameString.hpp
// @author F Prime
// @brief  A string sized to store a file name
// ======================================================================

#ifndef FW_FILE_NAME_STRING_HPP
#define FW_FILE_NAME_STRING_HPP

#include "Fw/Types/StringTemplate.hpp"
#include "config/FppConstantsAc.hpp"

namespace Fw {

using FileNameString = StringTemplate<FileNameStringSize>;

}  // namespace Fw

#endif
