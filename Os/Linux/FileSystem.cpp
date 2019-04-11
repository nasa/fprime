#include <Fw/Cfg/Config.hpp>
#include <Fw/Types/BasicTypes.hpp>
#include <Os/FileSystem.hpp>
#include <Os/File.hpp>
#include <Fw/Types/Assert.hpp>

#include <cerrno>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>
#include <stdio.h> // Needed for rename
#include <string.h>
#include <limits>
#include <sys/statvfs.h>

namespace Os {

	namespace FileSystem {

		Status createDirectory(const char* path) {

			Status stat = OP_OK;

#ifdef __VXWORKS__
			int mkStat = ::mkdir(path);
#else
			int mkStat = ::mkdir(path,S_IRWXU);
#endif

			if (-1 == mkStat) {
				switch (errno) {
					case EACCES:
					case EPERM:
					case EROFS:
					case EFAULT:
						stat = NO_PERMISSION;
						break;
					case EEXIST:
						stat = ALREADY_EXISTS;
						break;
					case ELOOP:
					case ENOENT:
					case ENAMETOOLONG:
						stat = INVALID_PATH;
						break;
					case ENOTDIR:
						stat = NOT_DIR;
						break;
					case ENOSPC:
					case EDQUOT:
						stat = NO_SPACE;
						break;
					case EMLINK:
						stat = FILE_LIMIT;
						break;
					default:
						stat = OTHER_ERROR;
						break;
				}
			}

			return stat;
		} // end createDirectory

		Status removeDirectory(const char* path) {

			Status stat = OP_OK;

			if (::rmdir(path) == -1) {
				switch (errno) {
					case EACCES:
					case EPERM:
					case EROFS:
					case EFAULT:
						stat = NO_PERMISSION;
						break;
					case ENOTEMPTY:
					case EEXIST:
						stat = NOT_EMPTY;
						break;
					case EINVAL:
					case ELOOP:
					case ENOENT:
					case ENAMETOOLONG:
						stat = INVALID_PATH;
						break;
					case ENOTDIR:
						stat = NOT_DIR;
						break;
					case EBUSY:
						stat = BUSY;
						break;
					default:
						stat = OTHER_ERROR;
						break;
				}
			}

			return stat;
		} // end removeDirectory

		Status readDirectory(const char* path, const U32 maxNum,
							 Fw::EightyCharString fileArray[],
							 U32& numFiles)
		{
			Status dirStat = OP_OK;
			DIR * dirPtr = NULL;
			struct dirent *direntData = NULL;

			FW_ASSERT(fileArray != NULL);
			FW_ASSERT(path != NULL);

			// Open directory failed:
			if((dirPtr = ::opendir(path)) == NULL) {

				switch (errno) {
					case EACCES:
						dirStat = NO_PERMISSION;
						break;
					case ENOENT:
						dirStat = INVALID_PATH;
						break;
					case ENOTDIR:
						dirStat = NOT_DIR;
						break;
					default:
						dirStat = OTHER_ERROR;
						break;
				}
				return dirStat;
			}

			// Set errno to 0 so we know why we exited readdir
			errno = 0;
			U32 arrayIdx = 0;
			U32 limitCount = 0;
			const U32 loopLimit = std::numeric_limits<U32>::max();

			// Read the directory contents and store in passed in array:
			while(arrayIdx < maxNum && limitCount < loopLimit) {

				++limitCount;

				if((direntData = ::readdir(dirPtr)) != NULL) {
					// We are only care about regular files
					if(direntData->d_type == DT_REG) {

						FW_ASSERT(arrayIdx < maxNum,
								  static_cast<NATIVE_INT_TYPE>(arrayIdx),
								  static_cast<NATIVE_INT_TYPE>(maxNum));

						Fw::EightyCharString str(direntData->d_name);
						fileArray[arrayIdx++] = str;
					}
				}
				else {
					// readdir failed, did it error or did we run out of files?
					if(errno != 0) {
						// Only error from readdir is EBADF
						dirStat = OTHER_ERROR;
					}
					break;
				}
			}

			if(limitCount == loopLimit) {
				dirStat = FILE_LIMIT;
			}

			if(::closedir(dirPtr) == -1) {
				// Only error from closedir is EBADF
				dirStat = OTHER_ERROR;
			}

			numFiles = arrayIdx;

			return dirStat;
		}

		Status removeFile(const char* path) {

			Status stat = OP_OK;

			if(::unlink(path) == -1) {
				switch (errno) {
					case EACCES:
					case EPERM:
					case EROFS:
						stat = NO_PERMISSION;
						break;
					case EISDIR:
						stat = IS_DIR;
						break;
					case ELOOP:
					case ENOENT:
					case ENAMETOOLONG:
						stat = INVALID_PATH;
						break;
					case ENOTDIR:
						stat = NOT_DIR;
						break;
					case EBUSY:
						stat = BUSY;
						break;
					default:
						stat = OTHER_ERROR;
						break;
				}
			}

			return stat;
		} // end removeFile

		Status moveFile(const char* originPath, const char* destPath) {

			Status stat = OP_OK;

			if(::rename(originPath, destPath) == -1) {
				switch (errno) {
					case EACCES:
					case EPERM:
					case EROFS:
					case EFAULT:
						stat = NO_PERMISSION;
						break;
					case EDQUOT:
					case ENOSPC:
						stat = NO_SPACE;
						break;
					case ELOOP:
					case ENAMETOOLONG:
					case ENOENT:
					case EINVAL:
						stat = INVALID_PATH;
						break;
					case ENOTDIR:
					case EISDIR: // Old path is not a dir
						stat = NOT_DIR;
						break;
					case ENOTEMPTY:
					case EEXIST:
						stat = NOT_EMPTY;
						break;
					case EMLINK:
						stat = FILE_LIMIT;
						break;
					case EBUSY:
						stat = BUSY;
						break;
					default:
						stat = OTHER_ERROR;
						break;
				}
			}
			return stat;

		} // end moveFile

		Status handleFileError(File::Status fileStatus) {
			Status fileSystemStatus = OTHER_ERROR;

			switch(fileStatus) {
				case File::NO_SPACE:
					fileSystemStatus = NO_SPACE;
					break;
				case File::NO_PERMISSION:
					fileSystemStatus = NO_PERMISSION;
					break;
				case File::DOESNT_EXIST:
					fileSystemStatus = INVALID_PATH;
					break;
				default:
					fileSystemStatus = OTHER_ERROR;
			}
			return fileSystemStatus;
		} // end handleFileError

		Status copyFile(const char* originPath, const char* destPath) {
			U8 fileBuffer[FILE_SYSTEM_CHUNK_SIZE];

			FileSystem::Status fs_status;
			struct stat file_info;
			File::Status file_status;

			U64 fileSize = 0;
			NATIVE_INT_TYPE chunkSize;

			File source;
			File destination;

			if(::stat(originPath, &file_info) == -1) {
				switch (errno) {
					case EACCES:
						fs_status = NO_PERMISSION;
						break;
					case ELOOP:
					case ENOENT:
					case ENAMETOOLONG:
						fs_status = INVALID_PATH;
						break;
					case ENOTDIR:
						fs_status = NOT_DIR;
						break;
					default:
						fs_status = OTHER_ERROR;
						break;
				}
				return fs_status;
			}

			// Make sure the origin is a regular file
			if(!S_ISREG(file_info.st_mode)) {
				return INVALID_PATH;
			}

			// Get the file size:
			fs_status = FileSystem::getFileSize(originPath, fileSize); //!< gets the size of the file (in bytes) at location path
			if(FileSystem::OP_OK != fs_status) {
				return fs_status;
			}

			file_status = source.open(originPath, File::OPEN_READ);
			if(file_status != File::OP_OK) {
				return handleFileError(file_status);
			}

			file_status = destination.open(destPath, File::OPEN_WRITE);
			if(file_status != File::OP_OK) {
				return handleFileError(file_status);
			}

			// Set loop limit
			const U64 copyLoopLimit = (((U64)fileSize/FILE_SYSTEM_CHUNK_SIZE)) + 2;

			U64 loopCounter = 0;
			while(loopCounter < copyLoopLimit) {
				chunkSize = FILE_SYSTEM_CHUNK_SIZE;
				file_status = source.read(&fileBuffer, chunkSize, false);
				if(file_status != File::OP_OK) {
					return handleFileError(file_status);
				}

				if(chunkSize == 0) {
					//file has been successfully copied
					break;
				}

				file_status = destination.write(fileBuffer, chunkSize, true);
				if(file_status != File::OP_OK) {
					return handleFileError(file_status);
				}
				loopCounter++;
			}
			FW_ASSERT(loopCounter < copyLoopLimit);

			(void) source.close();
			(void) destination.close();

			return FileSystem::OP_OK;
		} // end copyFile

		Status getFileSize(const char* path, U64& size) {

			Status fileStat = OP_OK;
			struct stat fileStatStruct;

			if(::stat(path, &fileStatStruct) == -1) {
				switch (errno) {
					case EACCES:
						fileStat = NO_PERMISSION;
						break;
					case ELOOP:
					case ENOENT:
					case ENAMETOOLONG:
						fileStat = INVALID_PATH;
						break;
					case ENOTDIR:
						fileStat = NOT_DIR;
						break;
					default:
						fileStat = OTHER_ERROR;
						break;
				}
				return fileStat;
			}

			size = fileStatStruct.st_size;

			return fileStat;
		} // end getFileSize

		Status changeWorkingDirectory(const char* path) {

			Status stat = OP_OK;

			if (::chdir(path) == -1) {
				switch (errno) {
					case EACCES:
					case EFAULT:
						stat = NO_PERMISSION;
						break;
					case ENOTEMPTY:
						stat = NOT_EMPTY;
						break;
					case ENOENT:
					case ELOOP:
					case ENAMETOOLONG:
						stat = INVALID_PATH;
						break;
					case ENOTDIR:
						stat = NOT_DIR;
						break;
					default:
						stat = OTHER_ERROR;
						break;
				}
			}

			return stat;
		} // end changeWorkingDirectory

		Status getFreeSpace(const char* path, U64& totalBytes, U64& freeBytes) {
			Status stat = OP_OK;

			struct statvfs fsStat;
			int ret = statvfs(path, &fsStat);
			if (ret) {
				switch (errno) {
					case EACCES:
						stat = NO_PERMISSION;
						break;
					case ELOOP:
					case ENOENT:
					case ENAMETOOLONG:
						stat = INVALID_PATH;
						break;
					case ENOTDIR:
						stat = NOT_DIR;
						break;
					default:
						stat = OTHER_ERROR;
						break;
				}
				return stat;
			}

			totalBytes = (U64) fsStat.f_blocks * (U64) fsStat.f_frsize;
			freeBytes = (U64) fsStat.f_bfree * (U64) fsStat.f_frsize;
			return stat;
		}

		// Public function to get the file count for a given directory.
		Status getFileCount (const char* directory, U32& fileCount) {
			Status dirStat = OP_OK;
			DIR * dirPtr = NULL;
			struct dirent *direntData = NULL;
			U32 limitCount;
			const U64 loopLimit = ((((U64) 1) << 32)-1); // Max value of U32

			fileCount = 0;
			if((dirPtr = ::opendir(directory)) == NULL) {
				switch (errno) {
					case EACCES:
						dirStat = NO_PERMISSION;
						break;
					case ENOENT:
						dirStat = INVALID_PATH;
						break;
					case ENOTDIR:
						dirStat = NOT_DIR;
						break;
					default:
						dirStat = OTHER_ERROR;
						break;
				}
				return dirStat;
			}

			// Set errno to 0 so we know why we exited readdir
			errno = 0;
			for(limitCount = 0; limitCount < loopLimit; limitCount++) {
				if((direntData = ::readdir(dirPtr)) != NULL) {
					// We are only counting regular files
					if(direntData->d_type == DT_REG) {
						fileCount++;
					}
				}
				else {
					// readdir failed, did it error or did we run out of files?
					if(errno != 0) {
						// Only error from readdir is EBADF
						dirStat = OTHER_ERROR;
					}
					break;
				}
			}
			if(limitCount == loopLimit) {
				dirStat = FILE_LIMIT;
			}

			if(::closedir(dirPtr) == -1) {
				// Only error from closedir is EBADF
				dirStat = OTHER_ERROR;
			}

			return dirStat;
		} //end getFileCount

	} // end FileSystem namespace

} // end Os namespace
