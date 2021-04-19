#include <Os/ValidateFile.hpp>
#include <Os/FileSystem.hpp>
#include <Os/File.hpp>
#include <Utils/Hash/HashBuffer.hpp>
#include <Fw/Types/Assert.hpp>

#include <stdio.h>

void testValidateFile() {

    Os::ValidateFile::Status validateStatus;
    Os::FileSystem::Status fsStatus;
    const char fileName[] = "mod.mk";
    const char nonexistentFileName[] = "thisfiledoesnotexist";
    const char hashFileName[] = "mod.mk.hashed";
    const char hardtoaccessHashFileName[] = "thisdirdoesnotexist/mod.mk.hashed";
    
    // Create a hash file:
    printf("Creating hash for file %s in %s\n", fileName, hashFileName);
    validateStatus = Os::ValidateFile::createValidation(fileName, hashFileName);   //!< create a validation of the file 'filename' and store it in
    if ( Os::ValidateFile::VALIDATION_OK != validateStatus ) {
        printf("\tFailed to hash file %s into hash file %s.\n", fileName, hashFileName);
        printf("\tReturn status: %d\n", validateStatus);
        FW_ASSERT(0);
        return;
    }

    // Create a hash of a file that doesn't exist:
    printf("Creating hash for file %s in %s\n", nonexistentFileName, hashFileName);
    validateStatus = Os::ValidateFile::createValidation(nonexistentFileName, hashFileName);   //!< create a validation of the file 'filename' and store it in
    if ( Os::ValidateFile::FILE_DOESNT_EXIST != validateStatus ) {
        printf("\tFile %s was found and hashed, but it shouldn't exist.\n", nonexistentFileName);
        printf("\tReturn status: %d\n", validateStatus);
        FW_ASSERT(0);
        return;
    }

    // Create a hash of a file that doesn't exist:
    printf("Creating hash for file %s in %s\n", fileName, hardtoaccessHashFileName);
    validateStatus = Os::ValidateFile::createValidation(fileName, hardtoaccessHashFileName);   //!< create a validation of the file 'filename' and store it in
    if ( Os::ValidateFile::VALIDATION_FILE_DOESNT_EXIST != validateStatus ) {
        printf("\tFile %s was found and hashed, but hash %s  shouldn't exist.\n", fileName, hardtoaccessHashFileName);
        printf("\tReturn status: %d\n", validateStatus);
        FW_ASSERT(0);
        return;
    }

    // Get file size:
    printf("Checking file size of %s.\n", hashFileName);
    U64 fileSize=0;
    fsStatus = Os::FileSystem::getFileSize(hashFileName, fileSize);
    if( Os::FileSystem::OP_OK != fsStatus ) {
    	printf("\tFailed to get file size of %s\n", hashFileName);
    	printf("\tReturn status: %d\n", fsStatus);
    	FW_ASSERT(0);
    	return;
    }
    Utils::HashBuffer buf;
    FW_ASSERT(fileSize == buf.getBuffCapacity());

    // Validate file:
    printf("Validating file %s against hash file %s\n", fileName, hashFileName);
    validateStatus = Os::ValidateFile::validate(fileName, hashFileName);   //!< create a validation of the file 'filename' and store it in
    if ( Os::ValidateFile::VALIDATION_OK != validateStatus ) {
        printf("\tFailed to validate file %s against hash file %s.\n", fileName, hashFileName);
        printf("\tReturn status: %d\n", validateStatus);
        FW_ASSERT(0);
        return;
    }

    // Validate bad file:
    printf("Validating file %s against hash file %s\n", fileName, fileName);
    validateStatus = Os::ValidateFile::validate(fileName, fileName);   //!< create a validation of the file 'filename' and store it in
    if ( Os::ValidateFile::VALIDATION_FAIL != validateStatus ) {
        printf("\tSucceeded in validating file %s against hash file %s. But this should fail.\n", fileName, fileName);
        printf("\tReturn status: %d\n", validateStatus);
        FW_ASSERT(0);
        return;
    }

    // Remove hash file:
    printf("Removing hash file %s\n", hashFileName);
    fsStatus = Os::FileSystem::removeFile(hashFileName);
    if( Os::FileSystem::OP_OK != fsStatus ) {
    	printf("\tFailed to remove file (%s)\n", hashFileName);
    	printf("\tReturn status: %d\n", fsStatus);
    	FW_ASSERT(0);
    	return;
    }
}

extern "C" {
    void validateFileTest(void);
}

void validateFileTest(void) {
    testValidateFile(); 
}
