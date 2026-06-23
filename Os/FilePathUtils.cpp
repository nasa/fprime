// ======================================================================
// \title Os/FilePathUtils.cpp
// \brief Implementation of file path utilities
// ======================================================================
#include <Fw/Types/Assert.hpp>
#include <Fw/Types/StringUtils.hpp>
#include <Os/FilePathUtils.hpp>
#include <Os/FileSystem.hpp>
#include <cstring>

namespace Os {
namespace FilePathUtils {

// Helper: validate that string_length did not return the buffer size
// (which indicates no null terminator was found — an invalid string).
static bool isValidCString(FwSizeType measuredLen, FwSizeType bufferLen) {
    return measuredLen < bufferLen;
}

// Helper: copy the raw (unresolved) absolute path into resolvedOut.
// If path is relative, it is prepended with baseDir.
static Status buildAbsolutePath(const char* path, const char* baseDir, char* resolvedOut, FwSizeType resolvedSize) {
    FW_ASSERT(path != nullptr);
    FW_ASSERT(resolvedOut != nullptr);

    if (path[0] != '/') {
        // Relative path: baseDir is required and must start with '/'
        if (baseDir == nullptr) {
            return INVALID_PATH;
        }
        const FwSizeType baseDirLen = Fw::StringUtils::string_length(baseDir, MAX_PATH_LENGTH);
        if (!isValidCString(baseDirLen, MAX_PATH_LENGTH)) {
            return INVALID_PATH;
        }
        if (baseDirLen == 0 || baseDir[0] != '/') {
            return INVALID_PATH;
        }
        const FwSizeType pathLen = Fw::StringUtils::string_length(path, MAX_PATH_LENGTH);
        if (!isValidCString(pathLen, MAX_PATH_LENGTH)) {
            return INVALID_PATH;
        }
        const FwSizeType needsSlash = (baseDir[baseDirLen - 1] != '/') ? 1 : 0;
        if ((baseDirLen + needsSlash + pathLen + 1) > resolvedSize) {
            return TOO_LONG;
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
        if (!isValidCString(pathLen, MAX_PATH_LENGTH)) {
            return INVALID_PATH;
        }
        if (pathLen + 1 > resolvedSize) {
            return TOO_LONG;
        }
        (void)std::memcpy(resolvedOut, path, pathLen + 1);
    }
    return VALID;
}

// Helper: resolve `.`, `..`, and `//` in place within resolvedOut.
//
// Algorithm: single-pass using read (readPos) and write (writePos) pointers
// on the same buffer. Since resolving only shrinks or maintains length,
// writePos <= readPos always holds, making memmove safe for overlapping regions.
//
// For each path segment between '/' separators:
//   - Empty or `.` segments are skipped (no-op)
//   - `..` segments back up writePos to the previous '/'
//   - Normal segments are shifted left (via memmove) to writePos
static Status resolveInPlace(char* resolvedOut) {
    FW_ASSERT(resolvedOut != nullptr);
    FW_ASSERT(resolvedOut[0] == '/');

    const FwSizeType pathLength = Fw::StringUtils::string_length(resolvedOut, MAX_PATH_LENGTH);
    if (!isValidCString(pathLength, MAX_PATH_LENGTH)) {
        return INVALID_PATH;
    }

    FwSizeType writePos = 1;  // position past root '/'

    for (FwSizeType readPos = 1; readPos <= pathLength;) {
        // Mark start of current segment
        FwSizeType segStart = readPos;

        // Scan forward to find the end of this segment (next '/' or end of string)
        for (; readPos < pathLength && resolvedOut[readPos] != '/'; readPos++) {
        }
        FwSizeType segLen = readPos - segStart;

        // Advance readPos past the '/' separator (or force exit at end)
        if (readPos < pathLength) {
            readPos++;
        } else {
            readPos = pathLength + 1;
        }

        // Skip empty segments (caused by consecutive slashes like "//")
        if (segLen == 0) {
            continue;
        }
        // Skip `.` segments (current directory, no-op)
        if (segLen == 1 && resolvedOut[segStart] == '.') {
            continue;
        }
        // Handle `..` segments by backing up to the previous directory
        if (segLen == 2 && resolvedOut[segStart] == '.' && resolvedOut[segStart + 1] == '.') {
            if (writePos > 1) {
                writePos--;
                for (; writePos > 1 && resolvedOut[writePos - 1] != '/'; writePos--) {
                }
            }
            continue;
        }

        // Normal segment: shift left to writePos
        (void)std::memmove(&resolvedOut[writePos], &resolvedOut[segStart], segLen);
        writePos += segLen;
        resolvedOut[writePos++] = '/';
    }

    // Remove trailing '/' (unless path is root "/")
    if (writePos > 1) {
        writePos--;
    }
    resolvedOut[writePos] = '\0';
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

Status resolvePath(const Fw::ConstStringBase& path, const Fw::ConstStringBase& baseDir, Fw::StringBase& resolvedOut) {
    // SAFETY: write directly into resolvedOut's internal buffer to avoid a temporary copy.
    // resolvePath always null-terminates the output, keeping the StringBase in a valid state.
    char* outBuffer = const_cast<char*>(resolvedOut.toChar());
    const FwSizeType capacity = static_cast<FwSizeType>(resolvedOut.getCapacity());
    return resolvePath(path.toChar(), baseDir.toChar(), outBuffer, capacity);
}

// Internal containment check on already-resolved paths.
// Verifies that resolvedPath starts with allowedDirectory as a prefix,
// and that the match occurs at a `/` boundary.
static Status checkContainment(const char* resolvedPath, const char* allowedDirectory) {
    FW_ASSERT(resolvedPath != nullptr);
    FW_ASSERT(allowedDirectory != nullptr);

    const FwSizeType allowedLen = Fw::StringUtils::string_length(allowedDirectory, MAX_PATH_LENGTH);
    const FwSizeType pathLen = Fw::StringUtils::string_length(resolvedPath, MAX_PATH_LENGTH);

    if (!isValidCString(allowedLen, MAX_PATH_LENGTH) || !isValidCString(pathLen, MAX_PATH_LENGTH)) {
        return INVALID_PATH;
    }
    if (allowedLen == 0 || pathLen == 0) {
        return OUTSIDE_SANDBOX;
    }
    if (allowedDirectory[allowedLen - 1] != '/') {
        return OUTSIDE_SANDBOX;
    }

    // Special case: path equals the allowed directory without trailing '/'.
    // e.g. path="/data/uplink" matches allowedDirectory="/data/uplink/"
    // This is valid because the path IS the sandbox directory itself.
    if (pathLen == allowedLen - 1 && std::memcmp(resolvedPath, allowedDirectory, pathLen) == 0) {
        return VALID;
    }

    // Normal case: path must be at least as long as allowed and start with it
    if (pathLen < allowedLen) {
        return OUTSIDE_SANDBOX;
    }
    if (std::memcmp(resolvedPath, allowedDirectory, allowedLen) != 0) {
        return OUTSIDE_SANDBOX;
    }
    return VALID;
}

Status isSubDirectory(const char* path, const char* allowedDirectory) {
    if (path == nullptr || allowedDirectory == nullptr) {
        return INVALID_PATH;
    }

    // Normalize allowedDirectory so that segments like `.` or `..` don't cause
    // false rejections during the prefix comparison in checkContainment.
    char normalizedDir[MAX_PATH_LENGTH];
    const Status normStatus = resolvePath(allowedDirectory, "/", normalizedDir, MAX_PATH_LENGTH);
    if (normStatus != VALID) {
        return INVALID_PATH;
    }
    // Re-add trailing '/' that resolvePath strips
    const FwSizeType normLen = Fw::StringUtils::string_length(normalizedDir, MAX_PATH_LENGTH);
    if (normLen > 0 && normalizedDir[normLen - 1] != '/') {
        if (normLen + 2 > MAX_PATH_LENGTH) {
            return TOO_LONG;
        }
        normalizedDir[normLen] = '/';
        normalizedDir[normLen + 1] = '\0';
    }

    // Resolve path: relative paths use CWD as base, absolute paths need no base.
    char resolved[MAX_PATH_LENGTH];
    Status resolveStatus;
    if (path[0] != '/') {
        char cwdBuffer[MAX_PATH_LENGTH];
        const Os::FileSystem::Status cwdStatus = Os::FileSystem::getWorkingDirectory(cwdBuffer, MAX_PATH_LENGTH);
        if (cwdStatus != Os::FileSystem::Status::OP_OK) {
            return INVALID_PATH;
        }
        resolveStatus = resolvePath(path, cwdBuffer, resolved, MAX_PATH_LENGTH);
    } else {
        resolveStatus = resolvePath(path, "/", resolved, MAX_PATH_LENGTH);
    }
    if (resolveStatus != VALID) {
        return resolveStatus;
    }

    return checkContainment(resolved, normalizedDir);
}

Status isSubDirectory(const Fw::ConstStringBase& path, const Fw::ConstStringBase& allowedDirectory) {
    return isSubDirectory(path.toChar(), allowedDirectory.toChar());
}

}  // namespace FilePathUtils
}  // namespace Os
