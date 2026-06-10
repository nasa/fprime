// ======================================================================
// \title Os/FilePathUtils.cpp
// \brief Implementation of file path utilities
// ======================================================================
#include <Fw/Types/Assert.hpp>
#include <Fw/Types/StringUtils.hpp>
#include <Os/FilePathUtils.hpp>
#include <cstring>

namespace Os {
namespace FilePathUtils {

// Helper: copy the raw (unresolved) absolute path into resolvedOut
static Status buildAbsolutePath(const char* path, const char* baseDir, char* resolvedOut, FwSizeType resolvedSize) {
    FW_ASSERT(path != nullptr);
    FW_ASSERT(resolvedOut != nullptr);

    if (path[0] != '/') {
        if (baseDir == nullptr || baseDir[0] != '/') {
            return INVALID_PATH;
        }
        const FwSizeType baseDirLen = Fw::StringUtils::string_length(baseDir, MAX_PATH_LENGTH);
        const FwSizeType pathLen = Fw::StringUtils::string_length(path, MAX_PATH_LENGTH);
        const FwSizeType needsSlash = (baseDirLen > 0 && baseDir[baseDirLen - 1] != '/') ? 1 : 0;
        if (baseDirLen + needsSlash + pathLen + 1 > resolvedSize) {
            return INVALID_PATH;
        }
        (void)std::memcpy(resolvedOut, baseDir, baseDirLen);
        FwSizeType pos = baseDirLen;
        if (needsSlash) {
            resolvedOut[pos++] = '/';
        }
        (void)std::memcpy(&resolvedOut[pos], path, pathLen);
        pos += pathLen;
        resolvedOut[pos] = '\0';
    } else {
        const FwSizeType pathLen = Fw::StringUtils::string_length(path, MAX_PATH_LENGTH);
        if (pathLen + 1 > resolvedSize) {
            return INVALID_PATH;
        }
        (void)std::memcpy(resolvedOut, path, pathLen + 1);
    }
    return VALID;
}

// Helper: resolve `.`, `..`, and `//` in place within resolvedOut.
// Uses a read pointer (r) and write pointer (w) on the same buffer.
// w <= r always holds since resolution only shrinks, so memmove is safe.
static Status resolveInPlace(char* resolvedOut) {
    FW_ASSERT(resolvedOut != nullptr);
    FW_ASSERT(resolvedOut[0] == '/');

    const FwSizeType len = Fw::StringUtils::string_length(resolvedOut, MAX_PATH_LENGTH);
    FwSizeType w = 1;  // write position (past root '/')

    for (FwSizeType r = 1; r <= len;) {
        FwSizeType segStart = r;
        for (; r < len && resolvedOut[r] != '/'; r++) {
        }
        FwSizeType segLen = r - segStart;
        if (r < len) {
            r++;
        } else {
            r = len + 1;
        }

        if (segLen == 0) {
            continue;
        }
        if (segLen == 1 && resolvedOut[segStart] == '.') {
            continue;
        }
        if (segLen == 2 && resolvedOut[segStart] == '.' && resolvedOut[segStart + 1] == '.') {
            if (w > 1) {
                w--;
                for (; w > 1 && resolvedOut[w - 1] != '/'; w--) {
                }
            }
            continue;
        }

        (void)std::memmove(&resolvedOut[w], &resolvedOut[segStart], segLen);
        w += segLen;
        resolvedOut[w++] = '/';
    }

    if (w > 1) {
        w--;
    }
    resolvedOut[w] = '\0';
    return VALID;
}

Status resolvePath(const char* path, const char* baseDir, char* resolvedOut, FwSizeType resolvedSize) {
    FW_ASSERT(resolvedOut != nullptr);
    if (path == nullptr || path[0] == '\0') {
        return INVALID_PATH;
    }

    const Status buildStatus = buildAbsolutePath(path, baseDir, resolvedOut, resolvedSize);
    if (buildStatus != VALID) {
        return buildStatus;
    }
    return resolveInPlace(resolvedOut);
}

// Internal containment check on already-resolved paths
static Status checkContainment(const char* resolvedPath, const char* allowedDirectory) {
    FW_ASSERT(resolvedPath != nullptr);
    FW_ASSERT(allowedDirectory != nullptr);

    const FwSizeType allowedLen = Fw::StringUtils::string_length(allowedDirectory, MAX_PATH_LENGTH);
    const FwSizeType pathLen = Fw::StringUtils::string_length(resolvedPath, MAX_PATH_LENGTH);

    if (allowedLen == 0 || pathLen == 0) {
        return OUTSIDE_SANDBOX;
    }
    if (allowedDirectory[allowedLen - 1] != '/') {
        return OUTSIDE_SANDBOX;
    }
    if (pathLen < allowedLen) {
        if (pathLen == allowedLen - 1 && std::memcmp(resolvedPath, allowedDirectory, pathLen) == 0) {
            return VALID;
        }
        return OUTSIDE_SANDBOX;
    }
    if (std::memcmp(resolvedPath, allowedDirectory, allowedLen) != 0) {
        return OUTSIDE_SANDBOX;
    }
    return VALID;
}

Status isSubDirectory(const char* path, const char* allowedDirectory, char* resolvedOut, FwSizeType resolvedSize) {
    if (path == nullptr || allowedDirectory == nullptr) {
        return INVALID_PATH;
    }

    // Normalize allowedDirectory so that segments like `.` or `..` don't cause
    // false rejections during the prefix comparison in checkContainment.
    char normalizedDir[MAX_PATH_LENGTH];
    const Status normStatus = resolvePath(allowedDirectory, nullptr, normalizedDir, MAX_PATH_LENGTH);
    if (normStatus != VALID) {
        return INVALID_PATH;
    }
    // Re-add trailing '/' that resolvePath strips
    const FwSizeType normLen = Fw::StringUtils::string_length(normalizedDir, MAX_PATH_LENGTH);
    if (normLen > 0 && normalizedDir[normLen - 1] != '/') {
        if (normLen + 2 > MAX_PATH_LENGTH) {
            return INVALID_PATH;
        }
        normalizedDir[normLen] = '/';
        normalizedDir[normLen + 1] = '\0';
    }

    char resolved[MAX_PATH_LENGTH];
    const Status resolveStatus = resolvePath(path, normalizedDir, resolved, MAX_PATH_LENGTH);
    if (resolveStatus != VALID) {
        return resolveStatus;
    }

    const Status containStatus = checkContainment(resolved, normalizedDir);
    if (containStatus != VALID) {
        return containStatus;
    }

    // Copy resolved path to output if requested
    if (resolvedOut != nullptr) {
        const FwSizeType resolvedLen = Fw::StringUtils::string_length(resolved, MAX_PATH_LENGTH);
        FW_ASSERT(resolvedSize > resolvedLen);
        (void)std::memcpy(resolvedOut, resolved, resolvedLen + 1);
    }

    return VALID;
}

}  // namespace FilePathUtils
}  // namespace Os
