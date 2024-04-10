// ======================================================================
// \title Os/Delegate.hpp
// \brief helper functions to ease correct getDelegate implementations
// ======================================================================
#include <new>
#include <type_traits>
#include "Fw/Types/Assert.hpp"
#include "Os/Os.hpp"
#ifndef OS_DELEGATE_HPP_
#define OS_DELEGATE_HPP_
namespace Os {
namespace Delegate {

//! \brief Make a delegate of type Interface using Implementation without copy-constructor support (generic function)
//!
//! This function is a generic implementation of the `getDelegate` functions provided for each function within Os. This
//! is templated over two types: Interface (e.g. TaskInterface) the interface the delegate supports, and Implementation
//! the implementation of the interface. This function takes care of the critical requirements of the getDelegate
//! function:
//!   1. Ensure Implementation is derived from Interface
//!   2. Ensure Implementation fits within FW_HANDLE_MAX_SIZE
//!   3. Ensure Implementation alignment fits within FW_HANDLE_ALIGNMENT
//!   4. Performs the correct placement new for normal constructors
//!   5. Ensure the returned pointer is not nullptr
//!
//! Implementors of a `getDelegate` function may use this function by calling it and returning the result.
//!
//! > Neither Interface nor Implementation is allowed to support copy-constructors
//!
//! Example: TaskInterface getDelegate (Without Copy-Constructor)
//!
//! ```c++
//! #include "Os/Delegate.hpp"
//!
//! namespace Os {
//! TaskInterface* TaskInterface::getDelegate(HandleStorage& aligned_new_memory) {
//!   return Os::Delegate::makeDelegate<TaskInterface, Os::Posix::Task::PosixTask>(aligned_new_memory);
//! }
//! }
//! ```
//! \tparam Interface: interface the delegate supports (e.g. TaskInterface)
//! \tparam Implementation: implementation class of the delegate (e.g. PosixTask)
//! \param aligned_new_memory: memory to be filled via placement new call
//! \return pointer to implementation result of placement new
template <class Interface, class Implementation>
inline Interface* makeDelegate(HandleStorage& aligned_new_memory) {
    // Ensure prerequisites before performing placement new
    static_assert(std::is_base_of<Interface, Implementation>::value, "Implementation must derive from Interface");
    static_assert(sizeof(Implementation) <= FW_HANDLE_MAX_SIZE, "Handle size not large enough");
    static_assert((FW_HANDLE_ALIGNMENT % alignof(Implementation)) == 0, "Handle alignment invalid");
    // Placement new the object and ensure non-null result
    Implementation* interface = new (aligned_new_memory) Implementation;
    FW_ASSERT(interface != nullptr);
    return interface;
}

//! \brief Make a delegate of type Interface using Implementation with copy-constructor support (generic function)
//!
//! This function is a generic implementation of the `getDelegate` functions provided for each function within Os. This
//! is templated over two types: Interface (e.g. TaskInterface) the interface the delegate supports, and Implementation
//! the implementation of the interface. This function takes care of the critical requirements of the getDelegate
//! function:
//!   1. Ensure Implementation is derived from Interface
//!   2. Ensure Implementation fits within FW_HANDLE_MAX_SIZE
//!   3. Ensure Implementation alignment fits within FW_HANDLE_ALIGNMENT
//!   4. Performs the correct placement new for normal and copy constructors
//!   5. Ensure the returned pointer is not nullptr
//!
//! Implementors of a `getDelegate` function may use this function by calling it and returning the result.
//!
//! Example: FileInterface getDelegate Supporting Copy-Constructor
//!
//! ```c++
//! #include "Os/Delegate.hpp"
//!
//! namespace Os {
//! FileInterface* FileInterface::getDelegate(HandleStorage& aligned_new_memory, const FileInterface* to_copy) {
//!   return Os::Delegate::makeDelegate<FileInterface, Os::Posix::File::PosixFile>(aligned_new_memory, to_copy);
//! }
//! }
//! ```
//! \tparam Interface: interface the delegate supports (e.g. FileInterface)
//! \tparam Implementation: implementation class of the delegate (e.g. PosixFile)
//! \param aligned_new_memory: memory to be filled via placement new call
//! \return pointer to implementation result of placement new
//! \param to_copy: pointer to Interface to be copied by copy constructor
//! \return pointer to implementation result of placement new
template <class Interface, class Implementation>
inline Interface* makeDelegate(HandleStorage& aligned_new_memory, const Interface* to_copy) {
    const Implementation* copy_me = reinterpret_cast<const Implementation*>(to_copy);
    // Ensure prerequisites before performing placement new
    static_assert(std::is_base_of<Interface, Implementation>::value, "Implementation must derive from Interface");
    static_assert(sizeof(Implementation) <= sizeof(aligned_new_memory), "Handle size not large enough");
    static_assert((FW_HANDLE_ALIGNMENT % alignof(Implementation)) == 0, "Handle alignment invalid");
    // Placement new the object and ensure non-null result
    Implementation* interface = nullptr;
    if (to_copy == nullptr) {
        interface = new (aligned_new_memory) Implementation;
    } else {
        interface = new (aligned_new_memory) Implementation(*copy_me);
    }
    FW_ASSERT(interface != nullptr);
    return interface;
}
}  // namespace Delegate
}  // namespace Os
#endif
