#include "StringUtils.hpp"
#include <Fw/Types/Assert.hpp>
#include <cstring>

char* Fw::StringUtils::string_copy(char* destination, const char* source, U32 num) {
    // Handle self-copy
    if(destination == source) {
        return destination;
    }

    // Copying a right overlapping range is undefined
    FW_ASSERT(destination < source ||  source + num < destination);

    char* returned = strncpy(destination, source, num);
    destination[num - 1] = '\0';
    return returned;
}
