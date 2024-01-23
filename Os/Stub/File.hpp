#include <Os/File.hpp>
#ifndef OS_STUB_FILE_HPP
#define OS_STUB_FILE_HPP
namespace Os {
namespace Stub {
namespace File {
/**
 * Handlers for stubbed file functions.  Singletons may be overwritten to change behavior for things like unit testing.
 */
namespace Handlers {
    //! Handler for construction calls
    typedef void (*ConstructHandler)();
    //! Handler for destruction calls
    typedef void (*DestructHandler)();
    //! Handler for open calls
    typedef Os::File::Status (*OpenHandler)(const char*, Os::File::Mode, bool);
    //! Handler for close calls
    typedef void (*CloseHandler)();
    //! Handler for size calls
    typedef Os::File::Status (*SizeHandler)(FwSizeType&);
    //! Handler for position calls
    typedef Os::File::Status (*PositionHandler)(FwSizeType&);
    //! Handler for preallocate calls
    typedef Os::File::Status (*PreallocateHandler)(FwSizeType, FwSizeType);
    //! Handler for seek calls
    typedef Os::File::Status (*SeekHandler)(FwSizeType, bool);
    //! Handler for flush calls
    typedef Os::File::Status (*FlushHandler)();
    //! Handler for read calls
    typedef Os::File::Status (*ReadHandler)(U8*, FwSizeType&, bool);
    //! Handler for write calls
    typedef Os::File::Status (*WriteHandler)(const void *, FwSizeType&, bool);

    //! Singleton construct handler
    extern ConstructHandler constructHandler;
    //! Singleton destruct handler
    extern DestructHandler destructHandler;
    //! Singleton open handler
    extern OpenHandler openHandler;
    //! Singleton close handler
    extern CloseHandler closeHandler;
    //! Singleton size handler
    extern SizeHandler sizeHandler;
    //! Singleton position handler
    extern PositionHandler positionHandler;
    //! Singleton preallocate handler
    extern PreallocateHandler preallocateHandler;
    //! Singleton seek handler
    extern SeekHandler seekHandler;
    //! Singleton flush handler
    extern FlushHandler flushHandler;
    //! Singleton read handler
    extern ReadHandler readHandler;
    //! Singleton write handler
    extern WriteHandler writeHandler;
};


} // namespace File
} // namespace Stub
} // namespace Os
#endif // OS_STUB_FILE_HPP