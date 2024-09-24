// ======================================================================
// \title  TraceFileLogger.cpp
// \author sreddy
// \brief  cpp file for TraceFileLogger component implementation class
// ======================================================================

#include "Svc/TraceFileLogger/TraceFileLogger.hpp"
#include "FpConfig.hpp"
#include <Fw/Types/Assert.hpp>
#include <Os/File.hpp>
#include <Os/FileSystem.hpp>
#include <limits>
#include <cstring>
#include <cstdio>
#include <Fw/Types/StringUtils.hpp>
#include <Fw/Types/BasicTypes.hpp>
#include <Fw/Trace/TraceEntity.hpp>

namespace Svc {

    // ----------------------------------------------------------------------
    // Component construction and destruction
    // ----------------------------------------------------------------------

    TraceFileLogger::TraceFileLogger(const char* const compName) : 
    TraceFileLoggerComponentBase(compName),
    m_log_file(),
    m_mode(CLOSED),
    m_fileName(),
    m_maxFileSize(0),
    m_byteCount(0),
    m_log_init(false),
    m_traceFilter(0)
    {
       this->m_enable_trace = (FW_TRACE_RECORD_TRACE == true) ? true : false;
       
       //Set data and size for file buffer
       this->m_file_buffer.setData(m_file_data);
       this->m_file_buffer.setSize(FW_TRACE_MAX_SER_SIZE);
    
       //Set array to process trace id filtering
       this->filterTraceId.set_array(traceId_array,sizeof(traceId_array));
    }

    TraceFileLogger::~TraceFileLogger() {
        if(OPEN == this->m_mode) {
            //Close file
            this->m_log_file.close();
            this->m_mode = CLOSED;
        }
    }

    void TraceFileLogger :: 
        init(NATIVE_INT_TYPE queueDepth, NATIVE_INT_TYPE instance)
    {
            TraceFileLoggerComponentBase::init(queueDepth,instance);
    }

    void TraceFileLogger::set_log_file(const char* fileName, const U32 maxSize) {
        
        FW_ASSERT(fileName != nullptr);

        if(this->m_enable_trace == true) {
            //If a file is already open then close it
            if(this->m_mode == OPEN) {
                this->m_mode = CLOSED;
                this->m_log_file.close();
            }

            //If file name is too large then return failure
            FwSizeType fileNameSize = Fw::StringUtils::string_length(fileName, static_cast<FwSizeType>(Fw::String::STRING_SIZE));
            if (fileNameSize == Fw::String::STRING_SIZE) {
                this->m_enable_trace = false;
            }
            
            Os::File::Status stat = this->m_log_file.open(fileName, Os::File::OPEN_CREATE, Os::File::OverwriteType::OVERWRITE);

            // Bad status when trying to open the file:
            if (stat != Os::File::OP_OK) {
                this->m_enable_trace = false;
                this->log_WARNING_LO_TraceFileOpenError(static_cast<Fw::LogStringArg>(fileName));
            }

            this->m_byteCount = 0;
            this->m_maxFileSize = maxSize;
            this->m_fileName = fileName;
            this->m_mode = OPEN;
        }
    }

    void TraceFileLogger::configure(const char* file, const U32 maxSize) {
        //printf("HERE1: Configure, set file name\n");
        FW_ASSERT(file != nullptr);
        this->set_log_file(file,maxSize);
    }

    void TraceFileLogger::filter(U16 traceType_bitmask, bool enable){
        if(traceType_bitmask == 0){
            //TODO: Figure out if this should be made an illegal entry or just ignore it?
            //      
        }
       
        if(enable == true) {
            this->m_traceFilter |= traceType_bitmask;
        }
        else { //disable
            U16 mask = 0xFFFF ^ traceType_bitmask;
            this->m_traceFilter &= mask;
        }

    }

    void TraceFileLogger::write_log_file(U8* data, U32 size) {

        //Write data to the trace logger
        FW_ASSERT(data != nullptr);

        if(this->m_enable_trace && this->m_mode == OPEN){ //&& size > 0){
            
            //Check if the file size exceeds
            U32 expected_byte_count = this->m_byteCount+size;
            if (expected_byte_count > this->m_maxFileSize) {
                //Rewrite file
                //Current design supports writing to a circular file
                //printf ("File will be overwritten. Seeking beginning of file\n");
                (void)this->m_log_file.seek(0,Os::FileInterface::SeekType::ABSOLUTE);
                this->m_byteCount = 0;
            }
            //else {
            FwSignedSizeType writeSize = size;
            FwSignedSizeType fileSize;
            (void) this->m_log_file.position(fileSize);
            //printf ("WRITING TO FILE HERE. File position is: %lld\n",fileSize);
            Os::File::Status stat = this->m_log_file.write(reinterpret_cast<const U8*>(data),writeSize,Os::File::WAIT);
            // Assert if file is not already open
            FW_ASSERT(stat != Os::File::NOT_OPENED);
            this->m_byteCount += (size);

            //}
        }
    }

    void TraceFileLogger::process_traceId_storage(U32 traceId,bool enable) {
        if (enable == false) {
            //Add trace ID to the list (if it doesn't already exist) to stop logging it
            (void)this->filterTraceId.add_element(traceId);
        }
        else{
            //Remove trace ID from list (if its in the list) to start logging it
            (void)this->filterTraceId.delete_element(traceId);
        }
    }

    // ----------------------------------------------------------------------
    // Handler implementations for user-defined typed input ports
    // ----------------------------------------------------------------------

    void TraceFileLogger::TraceBufferLogger_handler(FwIndexType portNum,
                                                FwTraceIdType id,
                                                Fw::Time& timeTag,
                                                const Fw::TraceCfg::TraceType& type,
                                                Fw::TraceBuffer& args) {
            
            FW_ASSERT(type.isValid());
            U16 bit_mask = static_cast<U16> (FILTER_BIT << type.e);
            U32 traceSize = 0;
            
            //Only log trace types that are enabled by either config or user
            if(!(this->m_traceFilter & bit_mask)) {
                //TODO: Should we generate an event here, letting user know that this specific filter is disabled?
                return;
            }
            //Only log trace Ids that're not filtered out  
            if(this->filterTraceId.search_array(id,NULL)) {
                return;
            }
           
           //Make a call to reset
           Fw::SerializeBufferBase& buf_ref = m_file_buffer.getSerializeRepr();
           buf_ref.resetSer();
           buf_ref.serialize(id);
           buf_ref.serialize(timeTag);
           if(FW_TRACE_RECORD_MINIMAL == false) {
            buf_ref.serialize(args);
            traceSize = m_file_buffer.getSize(); //Record max size of each trace record for circular file
           }
           else {
            traceSize = buf_ref.getBuffLength(); //Record only id & timetag
            printf("File size %d",buf_ref.getBuffLength());
           }
           //printf("Buffer size is :%d, Full buffer length: %d\n",m_file_buffer.getSize(),buf_ref.getBuffLength()); 
           //Note: Because its a circular file we're writing the full buffer capacatiy to the file
           //      instead of the actual buffer size (variable based on number of args). This will 
           //      ensure when the file is overwritten, we preserve old records
           this->write_log_file(m_file_buffer.getData(),traceSize);
           // If we choose not to use circular file write then use the below line instead. 
           //this->write_log_file(m_file_buffer.getData(),buf_ref.getBuffLength());
           
            /*
            //convert trace data to string to make it readable
            std::string args_text;
            args.toString(args_text);
            char textStr[FW_TRACE_BUFFER_MAX_SIZE];
            U32 packet_size = static_cast<U32> (snprintf(textStr,
                    FW_TRACE_BUFFER_MAX_SIZE,
                    "ID: %" PRI_FwTraceIdType" TimeBase: %" PRI_FwTimeBaseStoreType "Time:%" PRId32 ",%" PRId32 "Args: %s\n",
                    id, static_cast<FwTimeBaseStoreType>(timeTag.getTimeBase()),timeTag.getSeconds(),timeTag.getUSeconds(),args_text.c_str()));
            printf("Trace buffer logger invoked HERE\n");
            //textStr += args_text;
            this->write_log_file(reinterpret_cast<U8*>(textStr),packet_size);
            */
    }


    // ----------------------------------------------------------------------
    // Handler implementations for commands
    // ----------------------------------------------------------------------

    void TraceFileLogger ::EnableTrace_cmdHandler(FwOpcodeType opCode, U32 cmdSeq, Svc::TraceFileLogger_Enable enable) {
        
        FW_ASSERT(enable.isValid());
        this->m_enable_trace = enable; 
        this->cmdResponse_out(opCode, cmdSeq, Fw::CmdResponse::OK);
    }

    void TraceFileLogger ::DumpTraceDp_cmdHandler(FwOpcodeType opCode, U32 cmdSeq) {
        // TODO
        this->cmdResponse_out(opCode, cmdSeq, Fw::CmdResponse::OK);
    }
    void TraceFileLogger ::FilterTrace_cmdHandler(FwOpcodeType opCode,
                                          U32 cmdSeq,
                                          U16 bitmask,
                                          Svc::TraceFileLogger_Enable enable) {
        this->filter(bitmask,enable); 
        this->cmdResponse_out(opCode, cmdSeq, Fw::CmdResponse::OK);
    }

    void TraceFileLogger ::DisableTraceId_cmdHandler(FwOpcodeType opCode,
                                                 U32 cmdSeq,
                                                 U32 traceId,
                                                 Svc::TraceFileLogger_Enable enable) {
        //Add/remove trace IDs from the array
        this->process_traceId_storage(traceId,static_cast<bool>(enable)); 
        this->cmdResponse_out(opCode, cmdSeq, Fw::CmdResponse::OK);
    }
}  // namespace Svc
