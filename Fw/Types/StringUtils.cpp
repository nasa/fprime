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

FwSignedSizeType Fw::StringUtils::substring_find(const CHAR* source_string, FwSizeType source_size, const CHAR* sub_string, FwSizeType sub_size) {

    FW_ASSERT(source_string != nullptr);
    FW_ASSERT(sub_string != nullptr);

    // zero size sub-strings should always match
    if ((source_size > 0) && (0 == sub_size)) {
        return 0;
    }

    for (FwSizeType source_index = 0; source_index < source_size; source_index++) {
        // if the current character matches
        for (FwSizeType sub_index = 0; sub_index < sub_size; sub_index++) {
            // if there is a mismatch, go to next character
            if (source_string[source_index + sub_index] != sub_string[sub_index]) {
                break;
            } else if (sub_index == sub_size - 1) {
                // if we matched all the way to the end of the substring
                return static_cast<FwSignedSizeType>(source_index);
            }
        }
    }
    
    // if we make it here, no matches were found
    return -1;
}
