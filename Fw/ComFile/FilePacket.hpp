/*
 * FilePacket.hpp
 *
 *  Created on: January 21, 2015
 *      Author: Timothy Canham
 */

#ifndef FILEPACKET_HPP_
#define FILEPACKET_HPP_

#include <Fw/Com/ComPacket.hpp>
#include <Fw/Types/EightyCharString.hpp>
#include <Fw/ComFile/FileBuffer.hpp>
#include <Fw/Com/ComBuffer.hpp>

namespace Fw {

    class FileChunkPacket : public ComPacket {
        public:

    		enum FileSubType {
    			START_OF_FILE, // !< beginning of file indicator
				FILE_CHUNK, // !< a chunk of the file
				END_OF_FILE // !< end of file indicator
    		};

    		// fields from start/end of file packet
    		static bool getFileSubType(ComBuffer& buffer, FileSubType& type); // !< get the file subtype
    		static bool getFileName(ComBuffer& buffer, EightyCharString& name); // !< get the file subtype
    		static bool getFileSequenceTotal(ComBuffer& buffer, NATIVE_UINT_TYPE& total); // !< get total number of file chunks

    		// fields from a file chunk
    		static bool getFileSequenceNum(ComBuffer& buffer, NATIVE_UINT_TYPE& seq); // !< get current chunk number
    		static bool getFileChunk(ComBuffer& buffer, FileBuffer& chunk); // !< get current chunk number

        private:
    		FileChunkPacket();
            virtual ~FileChunkPacket();


    };

} /* namespace Fw */

#endif /* FILEPACKET_HPP_ */
