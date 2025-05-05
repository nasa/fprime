// ======================================================================
// \title Os/Stub/Console.hpp
// \brief stub implementation for Os::Console, header and test definitions
// ======================================================================
#include <cstdio>
#include <Os/Console.hpp>
#ifndef OS_Stub_Console_HPP
#define OS_Stub_Console_HPP

namespace Os {
namespace Stub {
namespace Console {

//! ConsoleHandle class definition for stub implementations.
//!
struct StubConsoleHandle : public ConsoleHandle {
};

//! \brief stub implementation of Os::ConsoleInterface
//!
//! Stub implementation of `ConsoleInterface` for use as a delegate class handling stub console operations.
//!
class StubConsole : public ConsoleInterface {
  public:
    //! \brief constructor
    //!
    StubConsole() = default;

    //! \brief copy constructor
    StubConsole(const StubConsole& other) = default;

    //! \brief default copy assignment
    StubConsole& operator=(const StubConsole& other) = default;

    //! \brief destructor
    //!
    ~StubConsole() override = default;

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
  private:
    //! File handle for PosixFile
    StubConsoleHandle m_handle;
};
} // namespace Console
} // namespace Stub
} // namespace Os

#endif // OS_Stub_Console_HPP
