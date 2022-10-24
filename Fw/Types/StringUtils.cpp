#include "StringUtils.hpp"
#include <Fw/Types/Assert.hpp>
#include <cstring>

char* Fw::StringUtils::string_copy(char* destination, const char* source, U32 num) {
    // Handle self-copy and 0 bytes copy
    if(destination == source || num == 0) {
        return destination;
    }

    // Copying an overlapping range is undefined
    U32 source_len = string_length(source, num) + 1;
    FW_ASSERT(source + source_len <= destination || destination + num <= source);

    char* returned = strncpy(destination, source, num);
    destination[num - 1] = '\0';
    return returned;
}

U32 Fw::StringUtils::string_length(const CHAR* source, U32 max_len) {
    U32 length = 0;
    FW_ASSERT(source != nullptr);
    for (length = 0; length < max_len; length++) {
        if (source[length] == '\0') {
            break;
        }
    }
    return length;
}
