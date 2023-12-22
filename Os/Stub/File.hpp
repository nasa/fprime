// ======================================================================
// \title Os/Stub/File.cpp
// \brief stub implementation for Os::File
// ======================================================================
#include <Os/File.hpp>

#ifndef OS_STUB_FILE_HPP
#define OS_STUB_FILE_HPP

namespace Os {
namespace Stub {

/**
 * Set the next status to be returned by the stubbed Os::File implementation.
 * @param status: status to be returned next
 */
void file_set_next_status(Os::File::Status status);

}
}

#endif
