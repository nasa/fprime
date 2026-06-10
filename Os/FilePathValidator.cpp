// ======================================================================
// \title Os/FilePathValidator.cpp
// \brief Implementation of file path validation utilities
// ======================================================================
#include <Fw/Types/Assert.hpp>
#include <Fw/Types/StringUtils.hpp>
#include <Os/FilePathValidator.hpp>
#include <cstring>

namespace Os {
namespace FilePathValidator {

// Segments are stored as offset+length pairs into the working buffer
struct PathSegment {
    FwSizeType offset;
    FwSizeType length;
};

Status resolvePath(const char* path, const char* baseDir, char* resolvedOut, FwSizeType resolvedSize) {
    FW_ASSERT(resolvedOut != nullptr);

    if (path == nullptr || path[0] == '\0') {
        return INVALID_PATH;
    }

    // Working buffer for the path we'll parse
    char workBuf[MAX_PATH_LENGTH];

    if (path[0] != '/') {
        // Relative path: prepend baseDir
        if (baseDir == nullptr || baseDir[0] != '/') {
            return INVALID_PATH;
        }
        const FwSizeType baseDirLen = Fw::StringUtils::string_length(baseDir, MAX_PATH_LENGTH);
        const FwSizeType pathLen = Fw::StringUtils::string_length(path, MAX_PATH_LENGTH);

        // Need room for baseDir + '/' + path + '\0'
        const FwSizeType needsSlash = (baseDirLen > 0 && baseDir[baseDirLen - 1] != '/') ? 1 : 0;
        if (baseDirLen + needsSlash + pathLen + 1 > MAX_PATH_LENGTH) {
            return INVALID_PATH;
        }
        (void)std::memcpy(workBuf, baseDir, baseDirLen);
        FwSizeType pos = baseDirLen;
        if (needsSlash) {
            workBuf[pos++] = '/';
        }
        (void)std::memcpy(&workBuf[pos], path, pathLen);
        pos += pathLen;
        workBuf[pos] = '\0';
    } else {
        // Absolute path: copy directly
        const FwSizeType pathLen = Fw::StringUtils::string_length(path, MAX_PATH_LENGTH);
        if (pathLen + 1 > MAX_PATH_LENGTH) {
            return INVALID_PATH;
        }
        (void)std::memcpy(workBuf, path, pathLen + 1);
    }

    // Parse the path into segments, resolving `.` and `..`
    // Maximum possible segments is bounded by path length / 2
    static constexpr FwSizeType MAX_SEGMENTS = MAX_PATH_LENGTH / 2;
    PathSegment segments[MAX_SEGMENTS];
    FwSizeType segmentCount = 0;

    const FwSizeType workLen = Fw::StringUtils::string_length(workBuf, MAX_PATH_LENGTH);
    FwSizeType i = 0;

    // Skip leading '/'
    if (workLen > 0 && workBuf[0] == '/') {
        i = 1;
    }

    while (i < workLen) {
        // Find the end of this segment
        FwSizeType segStart = i;
        while (i < workLen && workBuf[i] != '/') {
            i++;
        }
        FwSizeType segLen = i - segStart;

        // Skip the '/' delimiter
        if (i < workLen) {
            i++;
        }

        // Skip `.` (current directory)
        if (segLen == 1 && workBuf[segStart] == '.') {
            continue;
        }

        // Handle `..` (parent directory)
        if (segLen == 2 && workBuf[segStart] == '.' && workBuf[segStart + 1] == '.') {
            if (segmentCount > 0) {
                segmentCount--;
            }
            // At root, `..` is a no-op (can't go above root)
            continue;
        }

        // Skip empty segments (from `//`)
        if (segLen == 0) {
            continue;
        }

        // Store this segment
        if (segmentCount >= MAX_SEGMENTS) {
            return INVALID_PATH;
        }
        segments[segmentCount].offset = segStart;
        segments[segmentCount].length = segLen;
        segmentCount++;
    }

    // Reconstruct the resolved path
    FwSizeType outPos = 0;

    // Always start with '/'
    if (outPos + 1 >= resolvedSize) {
        return INVALID_PATH;
    }
    resolvedOut[outPos++] = '/';

    for (FwSizeType s = 0; s < segmentCount; s++) {
        const FwSizeType segLen = segments[s].length;
        // Need room for segment + '/' or '\0'
        if (outPos + segLen + 1 >= resolvedSize) {
            return INVALID_PATH;
        }
        (void)std::memcpy(&resolvedOut[outPos], &workBuf[segments[s].offset], segLen);
        outPos += segLen;
        resolvedOut[outPos++] = '/';
    }

    // Replace trailing '/' with '\0' (unless root path "/")
    if (outPos > 1) {
        outPos--;
    }
    resolvedOut[outPos] = '\0';

    return VALID;
}

Status checkContainment(const char* resolvedPath, const char* allowedDirectory) {
    FW_ASSERT(resolvedPath != nullptr);
    FW_ASSERT(allowedDirectory != nullptr);

    const FwSizeType allowedLen = Fw::StringUtils::string_length(allowedDirectory, MAX_PATH_LENGTH);
    const FwSizeType pathLen = Fw::StringUtils::string_length(resolvedPath, MAX_PATH_LENGTH);

    if (allowedLen == 0 || pathLen == 0) {
        return OUTSIDE_SANDBOX;
    }

    // Allowed directory must end with '/'
    if (allowedDirectory[allowedLen - 1] != '/') {
        return OUTSIDE_SANDBOX;
    }

    // Path must be at least as long as the allowed directory prefix
    if (pathLen < allowedLen) {
        // Special case: path equals allowed dir without trailing slash
        // e.g., path="/data/uplink" and allowed="/data/uplink/"
        if (pathLen == allowedLen - 1 && std::memcmp(resolvedPath, allowedDirectory, pathLen) == 0) {
            return VALID;
        }
        return OUTSIDE_SANDBOX;
    }

    // Check prefix match
    if (std::memcmp(resolvedPath, allowedDirectory, allowedLen) != 0) {
        return OUTSIDE_SANDBOX;
    }

    return VALID;
}

Status validatePath(const char* path, const char* allowedDirectory) {
    if (path == nullptr || allowedDirectory == nullptr) {
        return INVALID_PATH;
    }

    char resolved[MAX_PATH_LENGTH];
    const Status resolveStatus = resolvePath(path, allowedDirectory, resolved, MAX_PATH_LENGTH);
    if (resolveStatus != VALID) {
        return resolveStatus;
    }

    return checkContainment(resolved, allowedDirectory);
}

}  // namespace FilePathValidator
}  // namespace Os
