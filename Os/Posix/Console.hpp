// ======================================================================
// \title Os/Posix/Console.hpp
// \brief posix implementation for Os::Console, header and test definitions
// ======================================================================
#include <cstdio>
#include <Os/Console.hpp>
#ifndef OS_POSIX_Console_HPP
#define OS_POSIX_Console_HPP

namespace Os {
namespace Posix {
namespace Console {

//! ConsoleHandle class definition for posix implementations.
//!
struct PosixConsoleHandle : public ConsoleHandle {
    //! Posix console file descriptor
    FILE* m_file_descriptor = stdout;
};

//! \brief posix implementation of Os::ConsoleInterface
//!
//! Posix implementation of `ConsoleInterface` for use as a delegate class handling posix console operations. Posix
//! consoles write to either standard out or standard error. The default file descriptor used is standard out. This may
//! be changed by calling `setOutputStream`.
//!
class PosixConsole : public ConsoleInterface {
  public:
    //! Stream selection enumeration
    enum Stream {
        STANDARD_OUT = 0, //!< Use standard output stream
        STANDARD_ERROR = 1 //!< Use standard error stream
    };
    //! \brief constructor
    //!
    PosixConsole() = default;

    //! \brief copy constructor
    PosixConsole(const PosixConsole& other) = default;

    //! \brief assignment operator that copies the internal representation
    PosixConsole& operator=(const PosixConsole& other) = default;

    //! \brief destructor
    //!
    ~PosixConsole() override = default;

    // ------------------------------------
    // Functions overrides
    // ------------------------------------

    //! \brief write message to console
    //!
    //! Write a message to the console with a bounded size. This will use the active file descriptor as the output
    //! destination.
    //!
    //! \param message: raw message to write
    //! \param size: size of the message to write to the console
    void writeMessage(const CHAR *message, const FwSizeType size) override;


    //! \brief returns the raw console handle
    //!
    //! Gets the raw console handle from the implementation. Note: users must include the implementation specific
    //! header to make any real use of this handle. Otherwise it will be as an opaque type.
    //!
    //! \return raw console handle
    //!
    ConsoleHandle *getHandle() override;

    //! \brief select the output stream
    //!
    //! There are two streams defined: standard out, and standard error. This allows users of the posix log
    //! implementation to chose which stream to use.
    void setOutputStream(Stream stream);

  private:
    //! File handle for PosixFile
    PosixConsoleHandle m_handle;
};
} // namespace Console
} // namespace Posix
} // namespace Os

#endif // OS_POSIX_Console_HPP
