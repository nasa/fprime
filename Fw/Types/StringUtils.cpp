#include "StringUtils.hpp"
#include <Fw/Types/Assert.hpp>
#include "string.h"

char* Fw::StringUtils::string_copy(char* destination, const char* source, U32 num) {
    // Handle self-copy
    if(destination == source) {
        return destination;
    }

    // Copying an overlapping range is undefined
    FW_ASSERT(source < destination || destination + num <= source);

    char* returned = strncpy(destination, source, num);
    destination[num - 1] = '\0';
    return returned;
}
