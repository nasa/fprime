#include <FpConfig.hpp>
#include <Os/FileSystem.hpp>
#include <Os/File.hpp>
#include <Fw/Types/Assert.hpp>

namespace Os {
	namespace FileSystem {
		Status createDirectory(const char* path) {
			return NO_SPACE;
		} // end createDirectory

		Status removeDirectory(const char* path) {
			return INVALID_PATH;
		} // end removeDirectory

		Status readDirectory(const char* path, const U32 maxNum,
							 Fw::String fileArray[])
		{
			return OTHER_ERROR;
		}
		Status removeFile(const char* path) {

			return OTHER_ERROR;
		} // end removeFile

		Status moveFile(const char* originPath, const char* destPath) {
			return OTHER_ERROR;

		} // end moveFile

		Status handleFileError(File::Status fileStatus) {
			return OTHER_ERROR;
		} // end handleFileError

		Status copyFile(const char* originPath, const char* destPath) {
			return OTHER_ERROR;
		} // end copyFile

		Status getFileSize(const char* path, U64& size) {
			return OTHER_ERROR;
		} // end getFileSize

		Status changeWorkingDirectory(const char* path) {
			return OTHER_ERROR;
		} // end changeWorkingDirectory
		// Public function to get the file count for a given directory.
		Status getFileCount (const char* directory, U32& fileCount) {
			return OTHER_ERROR;
		} //end getFileCount
        Status appendFile(const char* originPath, const char* destPath, bool createMissingDest) {
            return OTHER_ERROR;
        }
        Status getFreeSpace(const char* path, U64& totalBytes, U64& freeBytes) {
            return OTHER_ERROR;
        }
	} // end FileSystem namespace
} // end Os namespace
