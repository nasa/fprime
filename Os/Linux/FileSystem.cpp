#include <FpConfig.hpp>
#include <Os/FileSystem.hpp>
#include <Os/File.hpp>
#include <Fw/Types/Assert.hpp>

#include <cerrno>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>
#include <cstdio> // Needed for rename
#include <cstring>
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
							 Fw::String fileArray[],
							 U32& numFiles)
		{
			Status dirStat = OP_OK;
			DIR * dirPtr = nullptr;
			struct dirent *direntData = nullptr;

			FW_ASSERT(fileArray != nullptr);
			FW_ASSERT(path != nullptr);

			// Open directory failed:
			if((dirPtr = ::opendir(path)) == nullptr) {

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

				if((direntData = ::readdir(dirPtr)) != nullptr) {
					// We are only care about regular files
					if(direntData->d_type == DT_REG) {

						FW_ASSERT(arrayIdx < maxNum,
								  static_cast<NATIVE_INT_TYPE>(arrayIdx),
								  static_cast<NATIVE_INT_TYPE>(maxNum));

						Fw::String str(direntData->d_name);
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

		/**
		 * A helper function that returns an "OP_OK" status if the given file
		 * exists and can be read from, otherwise returns an error status.
		 *
		 * If provided, will also initialize the given stat struct with file
		 * information.
		 */
		Status initAndCheckFileStats(const char* filePath,
									struct stat* fileInfo=nullptr) {
			FileSystem::Status fs_status;
			struct stat local_info;
			if(!fileInfo) {
				// No external struct given, so use the local one
				fileInfo = &local_info;
			}

			if(::stat(filePath, fileInfo) == -1) {
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
			if(!S_ISREG(fileInfo->st_mode)) {
				return INVALID_PATH;
			}

			return OP_OK;
		}

		/**
		 * A helper function that writes all the file information in the source
		 * file to the destination file (replaces/appends to end/etc. depending
		 * on destination file mode).
		 *
		 * Files must already be open and will remain open after this function
		 * completes.
		 *
		 * @param source File to copy data from
		 * @param destination File to copy data to
		 * @param size The number of bytes to copy
		 */
		Status copyFileData(File source, File destination, U64 size) {
			U8 fileBuffer[FILE_SYSTEM_CHUNK_SIZE];
			File::Status file_status;

			// Set loop limit
			const U64 copyLoopLimit = (size/FILE_SYSTEM_CHUNK_SIZE) + 2;

			U64 loopCounter = 0;
			NATIVE_INT_TYPE chunkSize;
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

			return FileSystem::OP_OK;
		} // end copyFileData

		Status copyFile(const char* originPath, const char* destPath) {
			FileSystem::Status fs_status;
			File::Status file_status;

			U64 fileSize = 0;

			File source;
			File destination;

			fs_status = initAndCheckFileStats(originPath);
			if(FileSystem::OP_OK != fs_status) {
				return fs_status;
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

			fs_status = copyFileData(source, destination, fileSize);

			(void) source.close();
			(void) destination.close();

			return fs_status;
		} // end copyFile

		Status appendFile(const char* originPath, const char* destPath, bool createMissingDest) {
			FileSystem::Status fs_status;
			File::Status file_status;
			U64 fileSize = 0;

			File source;
			File destination;

			fs_status = initAndCheckFileStats(originPath);
			if(FileSystem::OP_OK != fs_status) {
				return fs_status;
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

			// If needed, check if destination file exists (and exit if not)
			if(!createMissingDest) {
				fs_status = initAndCheckFileStats(destPath);
				if(FileSystem::OP_OK != fs_status) {
					return fs_status;
				}
			}

			file_status = destination.open(destPath, File::OPEN_APPEND);
			if(file_status != File::OP_OK) {
				return handleFileError(file_status);
			}

			fs_status = copyFileData(source, destination, fileSize);

			(void) source.close();
			(void) destination.close();

			return fs_status;
		} // end appendFile

		Status getFileSize(const char* path, U64& size) {

			Status fileStat = OP_OK;
			struct stat fileStatStruct;

			fileStat = initAndCheckFileStats(path, &fileStatStruct);
			if(FileSystem::OP_OK == fileStat) {
				// Only check size if struct was initialized successfully
				size = fileStatStruct.st_size;
			}

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

			totalBytes = static_cast<U64>(fsStat.f_blocks) * static_cast<U64>(fsStat.f_frsize);
			freeBytes = static_cast<U64>(fsStat.f_bfree) * static_cast<U64>(fsStat.f_frsize);
			return stat;
		}

		// Public function to get the file count for a given directory.
		Status getFileCount (const char* directory, U32& fileCount) {
			Status dirStat = OP_OK;
			DIR * dirPtr = nullptr;
			struct dirent *direntData = nullptr;
			U32 limitCount;
			const U64 loopLimit = std::numeric_limits<U32>::max();

			fileCount = 0;
			if((dirPtr = ::opendir(directory)) == nullptr) {
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
				if((direntData = ::readdir(dirPtr)) != nullptr) {
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
