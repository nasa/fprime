/**
 * File: Os/Log.hpp
 * Description: this file provides an implementation of the Fw::Logger class that is backed by the
 * Os abstraction layer.
 */
#ifndef Os_Console_hpp_
#define Os_Console_hpp_

#include <FpConfig.hpp>
#include <Fw/Logger/Logger.hpp>

namespace Os {
    //! \brief Base class for storing implementation specific handle information
    struct ConsoleHandle {
    };

    // \brief Interface defining the properties of the console
    class ConsoleInterface {
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
        virtual void write(const CHAR *message, const FwSizeType size) = 0;

        //! \brief returns the raw console handle
        //!
        //! Gets the raw console handle from the implementation. Note: users must include the implementation specific
        //! header to make any real use of this handle. Otherwise it will be as an opaque type.
        //!
        //! \return raw console handle
        //!
        virtual ConsoleHandle *getHandle();

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
        static ConsoleInterface* getDelegate(HandleStorage& aligned_placement_new_memory, const ConsoleInterface* to_copy=nullptr);
    };

    class Console : public ConsoleInterface, public Fw::Logger {
      public:
        //! \brief Default constructor
        Console();

        //! \brief Default destructor
        ~Console();

        //! \brief write message to console
        //!
        //! Write a message to the console with a bounded size. This will delegate to the implementation defined write
        //! method.
        //!
        //! \param message: raw message to write
        //! \param size: size of the message to write to the console
        void write(const CHAR *message, const FwSizeType size) override;

        //! \brief log a message using the format string and arguments
        //!
        //! Function called on the logger to log a message. This is abstract virtual method and must be supplied by the
        //! subclass. This logger object should be registered with the Fw::Log::registerLogger function.
        //! \param fmt: format string in which to place arguments
        //! \param a0: zeroth argument. (Default: 0)
        //! \param a1: first argument. (Default: 0)
        //! \param a2: second argument. (Default: 0)
        //! \param a3: third argument. (Default: 0)
        //! \param a4: fourth argument. (Default: 0)
        //! \param a5: fifth argument. (Default: 0)
        //! \param a6: sixth argument. (Default: 0)
        //! \param a7: seventh argument. (Default: 0)
        //! \param a8: eighth argument. (Default: 0)
        //! \param a9: ninth argument. (Default: 0)
        void log(const char *fmt,
                 POINTER_CAST a0 = 0,
                 POINTER_CAST a1 = 0,
                 POINTER_CAST a2 = 0,
                 POINTER_CAST a3 = 0,
                 POINTER_CAST a4 = 0,
                 POINTER_CAST a5 = 0,
                 POINTER_CAST a6 = 0,
                 POINTER_CAST a7 = 0,
                 POINTER_CAST a8 = 0,
                 POINTER_CAST a9 = 0
        ) override;
      private:
        // This section is used to store the implementation-defined console handle. To Os::Console and fprime, this type
        // is opaque and thus normal allocation cannot be done. Instead, we allow the implementor to store then handle
        // in the byte-array here and set `handle` to that address for storage.
        alignas(FW_HANDLE_ALIGNMENT) HandleStorage m_handle_storage; // Storage for the delegate
        ConsoleInterface &m_delegate; //!< Delegate for the real implementation
    };
}

#endif
