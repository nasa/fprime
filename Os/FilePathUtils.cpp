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

// Segments are stored as offset+length pairs into the working buffer
struct PathSegment {
    FwSizeType offset;
    FwSizeType length;
};

// Helper: build absolute working buffer from raw path + optional baseDir
static Status buildWorkBuf(const char* path, const char* baseDir, char* workBuf) {
    FW_ASSERT(path != nullptr);
    FW_ASSERT(workBuf != nullptr);

    if (path[0] != '/') {
        if (baseDir == nullptr || baseDir[0] != '/') {
            return INVALID_PATH;
        }
        const FwSizeType baseDirLen = Fw::StringUtils::string_length(baseDir, MAX_PATH_LENGTH);
        const FwSizeType pathLen = Fw::StringUtils::string_length(path, MAX_PATH_LENGTH);
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
        const FwSizeType pathLen = Fw::StringUtils::string_length(path, MAX_PATH_LENGTH);
        if (pathLen + 1 > MAX_PATH_LENGTH) {
            return INVALID_PATH;
        }
        (void)std::memcpy(workBuf, path, pathLen + 1);
    }
    return VALID;
}

// Helper: parse segments and reconstruct resolved path
static Status resolveSegments(const char* workBuf, char* resolvedOut, FwSizeType resolvedSize) {
    FW_ASSERT(workBuf != nullptr);
    FW_ASSERT(resolvedOut != nullptr);

    static constexpr FwSizeType MAX_SEGMENTS = MAX_PATH_LENGTH / 2;
    PathSegment segments[MAX_SEGMENTS];
    FwSizeType segmentCount = 0;

    const FwSizeType workLen = Fw::StringUtils::string_length(workBuf, MAX_PATH_LENGTH);
    FwSizeType start = (workLen > 0 && workBuf[0] == '/') ? 1 : 0;

    for (FwSizeType i = start; i <= workLen;) {
        FwSizeType segStart = i;
        for (; i < workLen && workBuf[i] != '/'; i++) {
        }
        FwSizeType segLen = i - segStart;
        if (i < workLen) {
            i++;
        } else {
            i = workLen + 1;  // exit
        }

        if (segLen == 0) {
            continue;
        }
        if (segLen == 1 && workBuf[segStart] == '.') {
            continue;
        }
        if (segLen == 2 && workBuf[segStart] == '.' && workBuf[segStart + 1] == '.') {
            if (segmentCount > 0) {
                segmentCount--;
            }
            continue;
        }
        if (segmentCount >= MAX_SEGMENTS) {
            return INVALID_PATH;
        }
        segments[segmentCount].offset = segStart;
        segments[segmentCount].length = segLen;
        segmentCount++;
    }

    // Reconstruct
    FwSizeType outPos = 0;
    if (outPos + 1 >= resolvedSize) {
        return INVALID_PATH;
    }
    resolvedOut[outPos++] = '/';

    for (FwSizeType s = 0; s < segmentCount; s++) {
        const FwSizeType segLen = segments[s].length;
        if (outPos + segLen + 1 >= resolvedSize) {
            return INVALID_PATH;
        }
        (void)std::memcpy(&resolvedOut[outPos], &workBuf[segments[s].offset], segLen);
        outPos += segLen;
        resolvedOut[outPos++] = '/';
    }

    if (outPos > 1) {
        outPos--;
    }
    resolvedOut[outPos] = '\0';
    return VALID;
}

Status resolvePath(const char* path, const char* baseDir, char* resolvedOut, FwSizeType resolvedSize) {
    FW_ASSERT(resolvedOut != nullptr);
    if (path == nullptr || path[0] == '\0') {
        return INVALID_PATH;
    }

    char workBuf[MAX_PATH_LENGTH];
    Status buildStatus = buildWorkBuf(path, baseDir, workBuf);
    if (buildStatus != VALID) {
        return buildStatus;
    }
    return resolveSegments(workBuf, resolvedOut, resolvedSize);
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

    char resolved[MAX_PATH_LENGTH];
    const Status resolveStatus = resolvePath(path, allowedDirectory, resolved, MAX_PATH_LENGTH);
    if (resolveStatus != VALID) {
        return resolveStatus;
    }

    const Status containStatus = checkContainment(resolved, allowedDirectory);
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
