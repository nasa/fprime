// ======================================================================
// \title Os/DelegateConsole.hpp
// \brief Define the Os::DelegateConsole class
// ======================================================================
#ifndef OS_DELEGATECONSOLE_HPP_
#define OS_DELEGATECONSOLE_HPP_

#include "Os/ConsoleInterface.hpp"

namespace Os {

class DelegateConsole final : public ConsoleInterface {
  public:
    //! \brief Default constructor
    DelegateConsole();

    //! \brief Default destructor
    ~DelegateConsole();

    //! \brief copy constructor that copies the internal representation
    DelegateConsole(const DelegateConsole& other);

    //! \brief assignment operator that copies the internal representation
    DelegateConsole& operator=(const DelegateConsole& other);

    //! \brief write message to console
    //!
    //! Write a message to the console with a bounded size. This will delegate to the implementation defined write
    //! method.
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

    //! \brief write message to console
    //!
    //! Write a message to the console as stored as a ConstStringBase type
    //!
    //! \param message: raw message to write (ConstStringBase)
    static void write(const Fw::ConstStringBase& message);

    //! \brief write message to the global console
    //!
    //! Write a message to the console with a bounded size. This will delegate to the global singleton
    //! implementation.
    //!
    //! \param message: raw message to write
    //! \param size: size of the message to write to the console
    static void write(const CHAR* message, const FwSizeType size);

    //! \brief initialize singleton
    static void init();

    //! \brief get a reference to singleton
    //! \return reference to singleton
    static DelegateConsole& getSingleton();

  private:
    // This section is used to store the implementation-defined console handle. To Os::Console and fprime, this type
    // is opaque and thus normal allocation cannot be done. Instead, we allow the implementor to store then handle
    // in the byte-array here and set `handle` to that address for storage.
    alignas(FW_HANDLE_ALIGNMENT) ConsoleHandleStorage m_handle_storage;  // Storage for the delegate
    ConsoleInterface& m_delegate;                                        //!< Delegate for the real implementation
};
}  // namespace Os

#endif  // OS_DELEGATECONSOLE_HPP_
