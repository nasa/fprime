/*
 * FileStubs.hpp
 *
 *  Created on: Sep 28, 2015
 *      Author: tcanham
 */

#ifndef STUBS_FILESTUBS_HPP_
#define STUBS_FILESTUBS_HPP_

#include <Os/File.hpp>

namespace Os {

    // Interceptors are used to intercept calls to the Os::File methods.
    // During a test, a callback can be registered with a pointer value
    // that is returned with the subsequent call. The pointer can be used
    // to store an instance of a test class to get back to the instance
    // that is doing the testing. If the interceptor returns true,
    // the regular call will be continued. This allows a particular
    // read in a sequence of reads to be modified while allowing previous
    // ones to complete normally.

    typedef bool (*OpenInterceptor)(Os::File::Status &status, const char* fileName, Os::File::Mode mode, void* ptr);
    typedef bool (*ReadInterceptor)(Os::File::Status &status, void * buffer, NATIVE_INT_TYPE &size, bool waitForFull, void* ptr);
    typedef bool (*WriteInterceptor)(Os::File::Status &status, const void * buffer, NATIVE_INT_TYPE &size, bool waitForDone, void* ptr);
    typedef bool (*SeekInterceptor)(Os::File::Status &status, NATIVE_INT_TYPE offset, bool absolute, void* ptr);

    void registerReadInterceptor(ReadInterceptor funcPtr, void* ptr);
    void clearReadInterceptor(void);

    void registerWriteInterceptor(WriteInterceptor funcPtr, void* ptr);
    void clearWriteInterceptor(void);

    void registerOpenInterceptor(OpenInterceptor funcPtr, void* ptr);
    void clearOpenInterceptor(void);

    void registerSeekInterceptor(SeekInterceptor funcPtr, void* ptr);
    void clearSeekInterceptor(void);

    void setLastError(NATIVE_INT_TYPE error);

}


#endif /* STUBS_FILESTUBS_HPP_ */
