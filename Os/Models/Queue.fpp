# ======================================================================
# \title Os/Models/Queue.fpp
# \brief FPP type definitions for Os/Queue.hpp concepts
# ======================================================================

module Os {
    @ FPP shadow-enum representing Os::Queue::Status
    enum QueueStatus {
        OP_OK,             @<  message sent/received okay
        ALREADY_CREATED,   @<  creating an already created queue
        EMPTY,             @<  If non-blocking, all the messages have been drained.
        UNINITIALIZED,     @<  Queue wasn't initialized successfully
        SIZE_MISMATCH,     @<  attempted to send or receive with buffer too large, too small
        SEND_ERROR,        @<  message send error
        RECEIVE_ERROR,     @<  message receive error
        INVALID_PRIORITY,  @<  invalid priority requested
        FULL,              @<  queue was full when attempting to send a message
        UNKNOWN_ERROR      @<  Unexpected error; can't match with returns
    }

    @ FPP shadow-enum representing Os::Queue::BlockingType
    enum QueueBlockingType {
        BLOCKING,    @< Message will block until space is available
        NONBLOCKING  @< Message will return with status when space is unavailable
    }
}
