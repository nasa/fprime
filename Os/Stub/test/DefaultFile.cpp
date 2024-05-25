// ======================================================================
// \title Os/Stub/test/DefaultFile.cpp
// \brief sets default Os::File to test stub implementation via linker
// ======================================================================
#include "Os/Stub/test/File.hpp"
#include "Os/Delegate.hpp"

namespace Os {

//! \brief get a delegate for FileInterface that intercepts calls for  for stub test file usage
//! \param aligned_new_memory: aligned memory to fill
//! \param to_copy: pointer to copy-constructor input
//! \return: pointer to delegate
FileInterface *FileInterface::getDelegate(HandleStorage& aligned_placement_new_memory, const FileInterface* to_copy) {
    return Os::Delegate::makeDelegate<FileInterface, Os::Stub::File::Test::TestFile>(
            aligned_placement_new_memory, to_copy
    );
}
}
