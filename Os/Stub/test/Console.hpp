// ======================================================================
// \title Os/Stub/test/Console.hpp
// \brief test stub implementation for Os::Console, header and test definitions
// ======================================================================
#include <cstdio>
#include <Os/Console.hpp>
#ifndef OS_Stub_Test_Console_HPP
#define OS_Stub_Test_Console_HPP

namespace Os {
namespace Stub {
namespace Console {
namespace Test {

class TestConsole;

//! Data that supports the stubbed File implementation.
//!/
struct StaticData {
    enum LastFn {
        NONE_FN,
        CONSTRUCT_FN,
        CONSTRUCT_COPY_FN,
        COPY_FN,
        DESTRUCT_FN,
        WRITE_FN,
    };
    //! Last function called
    LastFn lastCalled = NONE_FN;
    //! Copy object
    const TestConsole* copyObject;

    //! Last message passed
    const CHAR* message = nullptr;
    //! Last size passed
    FwSizeType size = 0;

    // Singleton data
    static StaticData data;
};

//! ConsoleHandle class definition for stub implementations.
//!
struct TestConsoleHandle : public ConsoleHandle {
};

//! \brief stub implementation of Os::ConsoleInterface
//!
//! Stub implementation of `ConsoleInterface` for use as a delegate class handling stub console operations.
//!
class TestConsole : public ConsoleInterface {
  public:
    //! \brief constructor
    //!
    TestConsole();

    //! \brief copy constructor
    TestConsole(const TestConsole& other);

    //! \brief assignment operator that copies the internal representation
    TestConsole& operator=(const TestConsole& other) = delete;

    //! \brief destructor
    //!
    ~TestConsole() override;

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
    void writeMessage(const CHAR* message, const FwSizeType size) override;

    //! \brief returns the raw console handle
    //!
    //! Gets the raw console handle from the implementation. Note: users must include the implementation specific
    //! header to make any real use of this handle. Otherwise it will be as an opaque type.
    //!
    //! \return raw console handle
    //!
    ConsoleHandle* getHandle() override;

  private:
    //! File handle for PosixFile
    TestConsoleHandle m_handle;
};
} // namespace Test
} // namespace Console
} // namespace Stub
} // namespace Os

#endif // OS_Stub_Test_Console_HPP
