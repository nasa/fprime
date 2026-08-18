#include <Utils/Hash/Hash.hpp>

namespace Utils {

const char* Hash ::getFileExtensionString() {
    return HASH_EXTENSION_STRING;
}

Fw::FormatStatus Hash ::addFileExtension(const Fw::ConstStringBase& baseName, Fw::StringBase& extendedName) {
    return extendedName.format("%s%s", baseName.toChar(), HASH_EXTENSION_STRING);
}

FwSizeType Hash ::getFileExtensionLength() {
    // Size of returns the size including the '\0' character.
    // We want to return just the size of the string.
    return sizeof(HASH_EXTENSION_STRING) - 1;
}

}  // namespace Utils
