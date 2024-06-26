// ======================================================================
// \title Os/Models/Models.hpp
// \brief header used to validate Os/Models before use
// ======================================================================

#include "Os/Models/FileStatusEnumAc.hpp"
#include "Os/Models/FileModeEnumAc.hpp"
#include "Os/Models/TaskStatusEnumAc.hpp"
#include "Os/File.hpp"
#include "Os/Task.hpp"

#ifndef OS_MODELS_MODELS_HPP
#define OS_MODELS_MODELS_HPP

// Check consistency of every constant in the Os::File::Status enum
static_assert(static_cast<FwIndexType>(Os::File::Status::MAX_STATUS) ==
              static_cast<FwIndexType>(Os::FileStatus::NUM_CONSTANTS),
              "File status and FPP shadow enum have inconsistent number of values");
static_assert(static_cast<Os::FileStatus::T>(Os::File::Status::OP_OK) == Os::FileStatus::T::OP_OK,
              "File status and FPP shadow enum do not match");
static_assert(static_cast<Os::FileStatus::T>(Os::File::Status::DOESNT_EXIST) == Os::FileStatus::T::DOESNT_EXIST,
              "File status and FPP shadow enum do not match");
static_assert(static_cast<Os::FileStatus::T>(Os::File::Status::NO_SPACE) == Os::FileStatus::T::NO_SPACE,
              "File status and FPP shadow enum do not match");
static_assert(static_cast<Os::FileStatus::T>(Os::File::Status::NO_PERMISSION) == Os::FileStatus::T::NO_PERMISSION,
              "File status and FPP shadow enum do not match");
static_assert(static_cast<Os::FileStatus::T>(Os::File::Status::BAD_SIZE) == Os::FileStatus::T::BAD_SIZE,
              "File status and FPP shadow enum do not match");
static_assert(static_cast<Os::FileStatus::T>(Os::File::Status::NOT_OPENED) == Os::FileStatus::T::NOT_OPENED,
              "File status and FPP shadow enum do not match");
static_assert(static_cast<Os::FileStatus::T>(Os::File::Status::FILE_EXISTS) == Os::FileStatus::T::FILE_EXISTS,
              "File status and FPP shadow enum do not match");
static_assert(static_cast<Os::FileStatus::T>(Os::File::Status::NOT_SUPPORTED) == Os::FileStatus::T::NOT_SUPPORTED,
              "File status and FPP shadow enum do not match");
static_assert(static_cast<Os::FileStatus::T>(Os::File::Status::INVALID_MODE) == Os::FileStatus::T::INVALID_MODE,
              "File status and FPP shadow enum do not match");
static_assert(static_cast<Os::FileStatus::T>(Os::File::Status::INVALID_ARGUMENT) == Os::FileStatus::T::INVALID_ARGUMENT,
              "File status and FPP shadow enum do not match");
static_assert(static_cast<Os::FileStatus::T>(Os::File::Status::OTHER_ERROR) == Os::FileStatus::T::OTHER_ERROR,
              "File status and FPP shadow enum do not match");

// Check consistency of every constant in the Os::File::Mode enum
static_assert(static_cast<FwIndexType>(Os::File::Mode::MAX_OPEN_MODE) ==
              static_cast<FwIndexType>(Os::FileMode::NUM_CONSTANTS),
              "File mode and FPP shadow enum have inconsistent number of values");
static_assert(static_cast<Os::FileMode::T>(Os::File::Mode::OPEN_NO_MODE) == Os::FileMode::T::OPEN_NO_MODE,
              "File mode and FPP shadow enum do not match");
static_assert(static_cast<Os::FileMode::T>(Os::File::Mode::OPEN_READ) == Os::FileMode::T::OPEN_READ,
              "File mode and FPP shadow enum do not match");
static_assert(static_cast<Os::FileMode::T>(Os::File::Mode::OPEN_CREATE) == Os::FileMode::T::OPEN_CREATE,
              "File mode and FPP shadow enum do not match");
static_assert(static_cast<Os::FileMode::T>(Os::File::Mode::OPEN_WRITE) == Os::FileMode::T::OPEN_WRITE,
              "File mode and FPP shadow enum do not match");
static_assert(static_cast<Os::FileMode::T>(Os::File::Mode::OPEN_SYNC_WRITE) == Os::FileMode::T::OPEN_SYNC_WRITE,
              "File mode and FPP shadow enum do not match");
static_assert(static_cast<Os::FileMode::T>(Os::File::Mode::OPEN_APPEND) == Os::FileMode::T::OPEN_APPEND,
              "File mode and FPP shadow enum do not Mode");

static_assert(static_cast<Os::TaskStatus::T>(Os::Task::Status::OP_OK) == Os::TaskStatus::T::OP_OK,
              "Task status and FPP shadow enum do not match");
static_assert(static_cast<Os::TaskStatus::T>(Os::Task::Status::INVALID_HANDLE) == Os::TaskStatus::T::INVALID_HANDLE,
              "Task status and FPP shadow enum do not match");
static_assert(static_cast<Os::TaskStatus::T>(Os::Task::Status::INVALID_PARAMS) == Os::TaskStatus::T::INVALID_PARAMS,
              "Task status and FPP shadow enum do not match");
static_assert(static_cast<Os::TaskStatus::T>(Os::Task::Status::INVALID_STACK) == Os::TaskStatus::T::INVALID_STACK,
              "Task status and FPP shadow enum do not match");
static_assert(static_cast<Os::TaskStatus::T>(Os::Task::Status::UNKNOWN_ERROR) == Os::TaskStatus::T::UNKNOWN_ERROR,
              "Task status and FPP shadow enum do not match");
static_assert(static_cast<Os::TaskStatus::T>(Os::Task::Status::INVALID_AFFINITY) == Os::TaskStatus::T::INVALID_AFFINITY,
              "Task status and FPP shadow enum do not match");
static_assert(static_cast<Os::TaskStatus::T>(Os::Task::Status::DELAY_ERROR) == Os::TaskStatus::T::DELAY_ERROR,
              "Task status and FPP shadow enum do not match");
static_assert(static_cast<Os::TaskStatus::T>(Os::Task::Status::JOIN_ERROR) == Os::TaskStatus::T::JOIN_ERROR,
              "Task status and FPP shadow enum do not match");
static_assert(static_cast<Os::TaskStatus::T>(Os::Task::Status::ERROR_RESOURCES) == Os::TaskStatus::T::ERROR_RESOURCES,
              "Task status and FPP shadow enum do not match");
static_assert(static_cast<Os::TaskStatus::T>(Os::Task::Status::ERROR_PERMISSION) == Os::TaskStatus::T::ERROR_PERMISSION,
              "Task status and FPP shadow enum do not match");
static_assert(static_cast<Os::TaskStatus::T>(Os::Task::Status::INVALID_STATE) == Os::TaskStatus::T::INVALID_STATE,
              "Task status and FPP shadow enum do not match");

#endif // OS_MODELS_MODELS_HPP
