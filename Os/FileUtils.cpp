// ======================================================================
// \title  Os/FileUtils.cpp
// \brief  implementation for file utility operations
// ======================================================================

#include <Os/FileUtils.hpp>
#include <Fw/Types/Assert.hpp>
#include <cstring>

namespace Os {

namespace FileUtils {

Status resolveUnixPath(const char* source, char* destination, FwSizeType destinationSize) {
    FW_ASSERT(source != nullptr);
    FW_ASSERT(destination != nullptr);
    FW_ASSERT(destinationSize > 0);

    // Path must be absolute (start with '/')
    if (source[0] != '/') {
        return INVALID_PATH;
    }

    const FwSizeType sourceLength = static_cast<FwSizeType>(strnlen(source, destinationSize));

    // If the source string fills the entire buffer with no room for null terminator,
    // the path is too long for the destination
    if (sourceLength >= destinationSize) {
        return BUFFER_TOO_SMALL;
    }

    // destIndex tracks the current write position in the destination buffer.
    // We process the source path component-by-component without allocating memory.
    FwSizeType destIndex = 0;

    // Start with root '/'
    destination[destIndex++] = '/';

    FwSizeType srcIndex = 1;  // Skip the initial '/'

    while (srcIndex < sourceLength) {
        // Find the end of the current component (next '/' or end of string)
        FwSizeType componentStart = srcIndex;
        while (srcIndex < sourceLength && source[srcIndex] != '/') {
            srcIndex++;
        }
        FwSizeType componentLength = srcIndex - componentStart;

        // Skip the '/' separator
        if (srcIndex < sourceLength) {
            srcIndex++;
        }

        if (componentLength == 0) {
            // Empty component (e.g., double slash "//"), skip it
            continue;
        }

        if (componentLength == 1 && source[componentStart] == '.') {
            // Current directory ".", skip it
            continue;
        }

        if (componentLength == 2 && source[componentStart] == '.' && source[componentStart + 1] == '.') {
            // Parent directory "..", go up one level
            // Move destIndex back past the previous component (but not before root)
            if (destIndex > 1) {
                // Walk back over the current component characters
                while (destIndex > 1 && destination[destIndex - 1] != '/') {
                    destIndex--;
                }
                // Walk back over the separator '/' (but not the root '/')
                if (destIndex > 1) {
                    destIndex--;
                }
            }
            // If destIndex == 1, we're already at root, stay there
            continue;
        }

        // Regular component: add separator if not right after root, then copy component
        // Calculate needed space: separator (if needed) + component + null terminator
        FwSizeType separatorSize = (destIndex > 1) ? 1 : 0;
        if (destIndex + separatorSize + componentLength + 1 > destinationSize) {
            return BUFFER_TOO_SMALL;
        }

        // Add '/' separator if not right after root
        if (destIndex > 1) {
            destination[destIndex++] = '/';
        }

        // Copy the component
        (void)memcpy(&destination[destIndex], &source[componentStart], componentLength);
        destIndex += componentLength;
    }

    // Null terminate
    destination[destIndex] = '\0';

    return OP_OK;
}

Status resolveUnixPath(const Fw::StringBase& source, Fw::StringBase& destination) {
    // Resolve using the char* implementation with the destination's internal buffer capacity
    char buffer[Fw::String::BUFFER_SIZE(Fw::String::STRING_SIZE)];
    FwSizeType bufferSize = static_cast<FwSizeType>(sizeof(buffer));

    Status status = resolveUnixPath(source.toChar(), buffer, bufferSize);
    if (status == OP_OK) {
        destination = buffer;
    }
    return status;
}

}  // namespace FileUtils

}  // namespace Os
