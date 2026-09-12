// ======================================================================
// \title Os/ConsoleInterface.hpp
// \brief Os::ConsoleHandle and Os::ConsoleInterface definitions
// ======================================================================
#ifndef OS_CONSOLEINTERFACE_HPP_
#define OS_CONSOLEINTERFACE_HPP_

#include <Fw/FPrimeBasicTypes.hpp>
#include <Fw/Logger/Logger.hpp>
#include <Os/Os.hpp>
#include "config/OsDelegateConsole.hpp"

namespace Os {
//! \brief Base class for storing implementation specific handle information
struct ConsoleHandle {};

// \brief Interface defining the properties of the console
class ConsoleInterface : public Fw::Logger {
  public:
    //! \brief Default constructor
    ConsoleInterface() = default;

    //! \brief Default destructor
    virtual ~ConsoleInterface() = default;

    //! \brief write message to console
    //!
    //! Write a message to the console with a bounded size.
    //!
    //! \param message: raw message to write
    //! \param size: size of the message to write to the console
    virtual void writeMessage(const CHAR* message, const FwSizeType size) = 0;

    //! \brief returns the raw console handle
    //!
    //! Gets the raw console handle from the implementation. Note: users must include the implementation specific
    //! header to make any real use of this handle. Otherwise it will be as an opaque type.
    //!
    //! \return raw console handle
    //!
    virtual ConsoleHandle* getHandle() = 0;

    //! \brief provide a pointer to a console delegate object
    //!
    //! This function must return a pointer to a `ConsoleInterface` object that contains the real implementation of
    //! the console functions as defined by the implementor.  This function must do several things to be considered
    //! correctly implemented:
    //!
    //! 1. Assert that their implementation fits within FW_HANDLE_MAX_SIZE.
    //!    e.g. `static_assert(sizeof(PosixFileImplementation) <= sizeof Os::File::m_handle_storage,
    //!        "FW_HANDLE_MAX_SIZE too small");`
    //! 2. Assert that their implementation aligns within FW_HANDLE_ALIGNMENT.
    //!    e.g. `static_assert((FW_HANDLE_ALIGNMENT % alignof(PosixFileImplementation)) == 0, "Bad handle alignment");`
    //! 3. If to_copy is null, placement new their implementation into `aligned_placement_new_memory`
    //!    e.g. `FileInterface* interface = new (aligned_placement_new_memory) PosixFileImplementation;`
    //! 4. If to_copy is non-null, placement new using copy constructor their implementation into
    //!    `aligned_placement_new_memory`
    //!    e.g. `FileInterface* interface = new (aligned_placement_new_memory) PosixFileImplementation(*to_copy);`
    //! 5. Return the result of the placement new
    //!    e.g. `return interface;`
    //!
    //! \return result of placement new, must be equivalent to `aligned_placement_new_memory`
    //!
    static ConsoleInterface* getDelegate(ConsoleHandleStorage& aligned_placement_new_memory,
                                         const ConsoleInterface* to_copy = nullptr);

    // ------------------------------------------------------------------
    // Common virtual functions built on top of OS-specific functions
    // ------------------------------------------------------------------

    //! \brief write message to console
    //!
    //! Write a message to the console as stored as a ConstStringBase type. Satisfies Fw::Logger's writeMessage
    //! contract by forwarding to the CHAR*-based console primitive.
    //!
    //! \param message: raw message to write (ConstStringBase)
    void writeMessage(const Fw::ConstStringBase& message) override;
};
}  // namespace Os

#endif  // OS_CONSOLEINTERFACE_HPP_
