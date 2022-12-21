#include <FpConfig.hpp>
#include <Os/File.hpp>
#include <Fw/Types/Assert.hpp>


namespace Os {

    File::File() :m_fd(0),m_mode(OPEN_NO_MODE),m_lastError(0) {}

    File::~File() {}

    File::Status File::open(const char* fileName, File::Mode mode) {
        return NOT_OPENED;
    }

    File::Status File::open(const char* fileName, File::Mode mode, bool include_excl) {
        return NOT_OPENED;
    }

    bool File::isOpen() {
      return false;
    }

    File::Status File::seek(NATIVE_INT_TYPE offset, bool absolute) {
        return NOT_OPENED;
    }

    File::Status File::read(void * buffer, NATIVE_INT_TYPE &size, bool waitForFull) {
        return NOT_OPENED;
    }

    File::Status File::write(const void * buffer, NATIVE_INT_TYPE &size, bool waitForDone) {
        return NOT_OPENED;
    }

    void File::close() {}

    NATIVE_INT_TYPE File::getLastError() {
        return 0;
    }

    const char* File::getLastErrorString() {
        return "FileBad";
    }

}
