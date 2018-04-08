#include <Utils/Hash/Hash.hpp>

namespace Utils {

    const char* Hash ::
        getFileExtensionString(void)
    {
        return HASH_EXTENSION_STRING;
    }

    void Hash ::
      addFileExtension(
          const Fw::EightyCharString& baseName,
          Fw::EightyCharString& extendedName
    ) {
      extendedName.format("%s%s", baseName.toChar(), HASH_EXTENSION_STRING);
    }

    NATIVE_UINT_TYPE Hash ::
        getFileExtensionLength(void)
    {
        // Size of returns the size including the '\0' character.
        // We want to return just the size of the string.
        return sizeof(HASH_EXTENSION_STRING) - 1;
    }

}
