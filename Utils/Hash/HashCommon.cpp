#include <Utils/Hash/Hash.hpp>

namespace Utils {

    const char* Hash ::
        getFileExtensionString()
    {
        return HASH_EXTENSION_STRING;
    }

    void Hash ::
      addFileExtension(
          const Fw::StringBase& baseName,
          Fw::StringBase& extendedName
    ) {
      extendedName.format("%s%s", baseName.toChar(), HASH_EXTENSION_STRING);
    }

    NATIVE_UINT_TYPE Hash ::
        getFileExtensionLength()
    {
        // Size of returns the size including the '\0' character.
        // We want to return just the size of the string.
        return sizeof(HASH_EXTENSION_STRING) - 1;
    }

}
