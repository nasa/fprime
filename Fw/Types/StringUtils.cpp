#include <Fw/Types/Assert.hpp>
#include <cstring>
#include "StringUtils.hpp"

char* Fw::StringUtils::string_copy(char* destination, const char* source, U32 num) {
    // Handle self-copy and 0 bytes copy
    if (destination == source || num == 0) {
        return destination;
    }
    FW_ASSERT(source != nullptr);
    FW_ASSERT(destination != nullptr);

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

I32 Fw::StringUtils::substring_find(const CHAR* source_string, U32 source_size, const CHAR* sub_string, U32 sub_size) {

    FW_ASSERT(source_string != nullptr);
    FW_ASSERT(sub_string != nullptr);

    for (U32 source_index = 0; source_index < source_size; source_index++) {
        // if the current character matches
        U32 dest_index = 0;
        if (source_string[source_index] == sub_string[dest_index]) {
            for (U32 sub_index = 0; sub_index < sub_size; sub_index++) {
                // if there is a mismatch, go to next character
                if (source_string[source_index + sub_index] != sub_string[sub_index]) {
                    // if we have reached the end of the source buffer, return match not found
                    if (source_index + sub_index == source_size-1) {
                        return -1;
                    }
                    // otherwise move to next character in source
                    continue;
                }
            }
            // if we got here, that means we matched all the way to the end of the substring
            return source_index;
        }
    }
    
    // if we make it here, no matches were found
    return -1;
}
