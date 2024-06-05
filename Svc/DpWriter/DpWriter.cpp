// ======================================================================
// \title  DpWriter.cpp
// \author bocchino
// \brief  cpp file for DpWriter component implementation class
// ======================================================================

#include "Fw/Com/ComPacket.hpp"
#include "Fw/Types/FileNameString.hpp"
#include "Fw/Types/Serializable.hpp"
#include "Os/File.hpp"
#include "Svc/DpWriter/DpWriter.hpp"
#include "Utils/Hash/Hash.hpp"
#include "config/DpCfg.hpp"
#include "config/FpConfig.hpp"

namespace Svc {

// ----------------------------------------------------------------------
// Construction, initialization, and destruction
// ----------------------------------------------------------------------

DpWriter::DpWriter(const char* const compName) : DpWriterComponentBase(compName), m_dpFileNamePrefix() {}

DpWriter::~DpWriter() {}

void DpWriter::configure(const Fw::StringBase& dpFileNamePrefix) {
    this->m_dpFileNamePrefix = dpFileNamePrefix;
}

// ----------------------------------------------------------------------
// Handler implementations for user-defined typed input ports
// ----------------------------------------------------------------------

void DpWriter::bufferSendIn_handler(const NATIVE_INT_TYPE portNum, Fw::Buffer& buffer) {
    Fw::Success::T status = Fw::Success::SUCCESS;
    // portNum is unused
    (void)portNum;
    // Update num buffers received
    ++this->m_numBuffersReceived;
    // Check that the buffer is valid
    if (!buffer.isValid()) {
        this->log_WARNING_HI_InvalidBuffer();
        status = Fw::Success::FAILURE;
    }
    // Check that the buffer is large enough to hold a data product packet
    const FwSizeType bufferSize = buffer.getSize();
    if (status == Fw::Success::SUCCESS) {
        if (bufferSize < Fw::DpContainer::MIN_PACKET_SIZE) {
            this->log_WARNING_HI_BufferTooSmallForPacket(
                static_cast<U32>(bufferSize),
                Fw::DpContainer::MIN_PACKET_SIZE);

            status = Fw::Success::FAILURE;
        }
    }
    // Set up the container and check that the header hash is valid
    Fw::DpContainer container;
    if (status == Fw::Success::SUCCESS) {
        container.setBuffer(buffer);
        Utils::HashBuffer storedHash;
        Utils::HashBuffer computedHash;
        status = container.checkHeaderHash(storedHash, computedHash);
        if (status != Fw::Success::SUCCESS) {
            this->log_WARNING_HI_InvalidHeaderHash(
                static_cast<U32>(bufferSize),
                storedHash.asBigEndianU32(),
                computedHash.asBigEndianU32());
        }
    }
    // Deserialize the packet header
    if (status == Fw::Success::SUCCESS) {
        status = this->deserializePacketHeader(buffer, container);
    }
    // Check that the packet size fits in the buffer
    if (status == Fw::Success::SUCCESS) {
        const FwSizeType packetSize = container.getPacketSize();
        if (bufferSize < packetSize) {
            this->log_WARNING_HI_BufferTooSmallForData(
                static_cast<U32>(bufferSize),
                static_cast<U32>(packetSize));
            status = Fw::Success::FAILURE;
        }
    }
    // Perform the requested processing
    if (status == Fw::Success::SUCCESS) {
        this->performProcessing(container);
    }
    // Construct the file name
    Fw::FileNameString fileName;
    if (status == Fw::Success::SUCCESS) {
        const FwDpIdType containerId = container.getId();
        const Fw::Time timeTag = container.getTimeTag();
        fileName.format(DP_FILENAME_FORMAT, this->m_dpFileNamePrefix.toChar(), containerId, timeTag.getSeconds(),
                        timeTag.getUSeconds());
    }
    FwSizeType fileSize = 0;
    // Write the file
    if (status == Fw::Success::SUCCESS) {
        status = this->writeFile(container, fileName, fileSize);
    }
    // Send the DpWritten notification
    if (status == Fw::Success::SUCCESS) {
        this->sendNotification(container, fileName, fileSize);
    }
    // Deallocate the buffer
    if (buffer.isValid()) {
        this->deallocBufferSendOut_out(0, buffer);
    }
    // Update the error count
    if (status != Fw::Success::SUCCESS) {
        this->m_numErrors++;
    }
}

void DpWriter::schedIn_handler(const NATIVE_INT_TYPE portNum, U32 context) {
    // portNum and context are not used
    (void)portNum;
    (void)context;
    // Write telemetry
    this->tlmWrite_NumBuffersReceived(this->m_numBuffersReceived);
    this->tlmWrite_NumBytesWritten(this->m_numBytesWritten);
    this->tlmWrite_NumSuccessfulWrites(this->m_numSuccessfulWrites);
    this->tlmWrite_NumFailedWrites(this->m_numFailedWrites);
    this->tlmWrite_NumErrors(this->m_numErrors);
}

// ----------------------------------------------------------------------
// Handler implementations for commands
// ----------------------------------------------------------------------

void DpWriter::CLEAR_EVENT_THROTTLE_cmdHandler(FwOpcodeType opCode, U32 cmdSeq) {
    // opCode and cmdSeq are not used
    (void)opCode;
    (void)cmdSeq;
    // Clear throttling
    this->log_WARNING_HI_BufferTooSmallForData_ThrottleClear();
    this->log_WARNING_HI_BufferTooSmallForPacket_ThrottleClear();
    this->log_WARNING_HI_FileOpenError_ThrottleClear();
    this->log_WARNING_HI_FileWriteError_ThrottleClear();
    this->log_WARNING_HI_InvalidBuffer_ThrottleClear();
    this->log_WARNING_HI_InvalidHeaderHash_ThrottleClear();
    this->log_WARNING_HI_InvalidHeader_ThrottleClear();
    // Return command response
    this->cmdResponse_out(opCode, cmdSeq, Fw::CmdResponse::OK);
}

// ----------------------------------------------------------------------
// Private helper functions
// ----------------------------------------------------------------------

Fw::Success::T DpWriter::deserializePacketHeader(Fw::Buffer& buffer, Fw::DpContainer& container) {
    Fw::Success::T status = Fw::Success::SUCCESS;
    container.setBuffer(buffer);
    const Fw::SerializeStatus serialStatus = container.deserializeHeader();
    if (serialStatus != Fw::FW_SERIALIZE_OK) {
        this->log_WARNING_HI_InvalidHeader(static_cast<U32>(buffer.getSize()), static_cast<U32>(serialStatus));
        status = Fw::Success::FAILURE;
    }
    return status;
}

void DpWriter::performProcessing(const Fw::DpContainer& container) {
    // Get the buffer
    Fw::Buffer buffer = container.getBuffer();
    // Get the bit mask for the processing types
    const Fw::DpCfg::ProcType::SerialType procTypes = container.getProcTypes();
    // Do the processing
    for (FwIndexType portNum = 0; portNum < NUM_PROCBUFFERSENDOUT_OUTPUT_PORTS; ++portNum) {
        if ((procTypes & (1 << portNum)) != 0) {
            this->procBufferSendOut_out(portNum, buffer);
        }
    }
}

Fw::Success::T DpWriter::writeFile(const Fw::DpContainer& container,
                                   const Fw::FileNameString& fileName,
                                   FwSizeType& fileSize) {
    Fw::Success::T status = Fw::Success::SUCCESS;
    // Get the buffer
    Fw::Buffer buffer = container.getBuffer();
    // Get the file size
    fileSize = container.getPacketSize();
    // Open the file
    Os::File file;
    Os::File::Status fileStatus = file.open(fileName.toChar(), Os::File::OPEN_CREATE);
    if (fileStatus != Os::File::OP_OK) {
        this->log_WARNING_HI_FileOpenError(static_cast<U32>(fileStatus), fileName);
        status = Fw::Success::FAILURE;
    }
    // Write the file
    if (status == Fw::Success::SUCCESS) {
        // Set write size to file size
        // On entry to the write call, this is the number of bytes to write
        // On return from the write call, this is the number of bytes written
        FwSignedSizeType writeSize = static_cast<FwSignedSizeType>(fileSize);
        fileStatus = file.write(buffer.getData(), writeSize);
        // If a successful write occurred, then update the number of bytes written
        if (fileStatus == Os::File::OP_OK) {
            this->m_numBytesWritten += static_cast<U64>(writeSize);
        }
        if ((fileStatus == Os::File::OP_OK) and (writeSize == static_cast<FwSignedSizeType>(fileSize))) {
            // If the write status is success, and the number of bytes written
            // is the expected number, then record the success
            this->log_ACTIVITY_LO_FileWritten(
                static_cast<U32>(writeSize),
                fileName);
        } else {
            // Otherwise record the failure
            this->log_WARNING_HI_FileWriteError(static_cast<U32>(fileStatus), static_cast<U32>(writeSize),
                                                static_cast<U32>(fileSize), fileName);
            status = Fw::Success::FAILURE;
        }
    }
    // Update the count of successful or failed writes
    if (status == Fw::Success::SUCCESS) {
        this->m_numSuccessfulWrites++;
    } else {
        this->m_numFailedWrites++;
    }
    // Return the status
    return status;
}

void DpWriter::sendNotification(const Fw::DpContainer& container,
                                const Fw::FileNameString& fileName,
                                FwSizeType fileSize) {
    if (isConnected_dpWrittenOut_OutputPort(0)) {
        // Get the priority
        const FwDpPriorityType priority = container.getPriority();
        this->dpWrittenOut_out(0, fileName, priority, fileSize);
    }
}

}  // end namespace Svc
