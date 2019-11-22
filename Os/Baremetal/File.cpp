#include <Fw/Cfg/Config.hpp>
#include <Fw/Types/BasicTypes.hpp>
#include <Os/File.hpp>
#include <Fw/Types/Assert.hpp>


namespace Os {

    File::File() :m_fd(0),m_mode(OPEN_NO_MODE),m_lastError(0) {}

    File::~File() {}

    File::Status File::open(const char* fileName, File::Mode mode) {}

    File::Status File::seek(NATIVE_INT_TYPE offset, bool absolute) {
        return NOT_OPENED;
    }

    File::Status File::read(void * buffer, NATIVE_INT_TYPE &size, bool waitForFull) {
        return NOT_OPENED;
    }

    File::Status File::write(const void * buffer, NATIVE_INT_TYPE &size, bool waitForDone) {
        return NOT_OPENED;
    }

    void File::close(void) {}

    NATIVE_INT_TYPE File::getLastError(void) {
        return 0;
    }

    const char* File::getLastErrorString(void) {
        return "FileBad";
    }

}
