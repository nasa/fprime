#ifndef _FileSystem_hpp_
#define _FileSystem_hpp_

#include <FpConfig.hpp>
#include <Fw/Types/String.hpp>

#define FILE_SYSTEM_CHUNK_SIZE (256u)

namespace Os {

	// This namespace encapsulates a very simple file system interface that has the most often-used features.
	namespace FileSystem {

		typedef enum {
			OP_OK, //!<  Operation was successful
			ALREADY_EXISTS, //!<  File already exists
			NO_SPACE, //!<  No space left
			NO_PERMISSION, //!<  No permission to write
			NOT_DIR, //!<  Path is not a directory
			IS_DIR, //!< Path is a directory
			NOT_EMPTY, //!<  directory is not empty
			INVALID_PATH, //!< Path is too long, too many sym links, doesn't exist, ect
			FILE_LIMIT, //!< Too many files or links
			BUSY, //!< Operand is in use by the system or by a process
			OTHER_ERROR, //!<  other OS-specific error
		} Status;

		Status createDirectory(const char* path); //!<  create a new directory at location path
		Status removeDirectory(const char* path); //!<  remove a directory at location path
		Status readDirectory(const char* path,  const U32 maxNum, Fw::String fileArray[], U32& numFiles);	//!< read the contents of a directory.  Size of fileArray should be maxNum. Cleaner implementation found in Directory.hpp
		Status removeFile(const char* path); //!< removes a file at location path
		Status moveFile(const char* originPath, const char* destPath); //! moves a file from origin to destination
		Status copyFile(const char* originPath, const char* destPath); //! copies a file from origin to destination
		Status appendFile(const char* originPath, const char* destPath, bool createMissingDest=false); //! append file origin to destination file. If boolean true, creates a brand new file if the destination doesn't exist.
		Status getFileSize(const char* path, U64& size); //!< gets the size of the file (in bytes) at location path
		Status getFileCount(const char* directory, U32& fileCount); //!< counts the number of files in the given directory
		Status changeWorkingDirectory(const char* path); //!<  move current directory to path

        Status getFreeSpace(const char* path, U64& totalBytes, U64& freeBytes); //!< get FS free and total space in bytes on filesystem containing path
	}

}

#endif
