#include "StringUtils.hpp"
#include "string.h"

char* Fw::StringUtils::string_copy(char* destination, const char* source, U32 num) {
    char* returned = strncpy(destination, source, num);
    destination[num - 1] = '\0';
    return returned;
}
