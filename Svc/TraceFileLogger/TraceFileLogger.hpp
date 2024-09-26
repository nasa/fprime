// ======================================================================
// \title  TraceFileLogger.hpp
// \author sreddy
// \brief  hpp file for TraceFileLogger component implementation class
// ======================================================================

#ifndef Svc_TraceFileLogger_HPP
#define Svc_TraceFileLogger_HPP

#include "Svc/TraceFileLogger/TraceFileLoggerComponentAc.hpp"
#include "Svc/TraceFileLogger/ArrayProc.hpp"
#include <Os/File.hpp>
#include <Fw/Types/Assert.hpp>
#include <Fw/Buffer/Buffer.hpp>

// some limits.h don't have PATH_MAX
#ifdef PATH_MAX
#define FILE_PATH_MAX PATH_MAX
#else
#define FILE_PATH_MAX 255
#endif

// some limits.h don't have NAME_MAX
#ifdef NAME_MAX
#define FILE_NAME_MAX NAME_MAX
#else
#define FILE_NAME_MAX 255
#endif

// Trace ID filter array size
#ifndef FILTER_TRACE_ID_SIZE 
#define FILTER_TRACE_ID_SIZE 10
#endif

//Max size of the Trace buffer including metadata (id,timetag,arguments) 
static const FwSizeType FW_TRACE_MAX_SER_SIZE = (FW_TRACE_BUFFER_MAX_SIZE + sizeof(FwTraceIdType) + Fw::Time::SERIALIZED_SIZE);

//Mask bit for filtering on trace types
static const U16 FILTER_BIT = 1;


namespace Svc {

class TraceFileLogger : public TraceFileLoggerComponentBase {
  public:
    // ----------------------------------------------------------------------
    // Component construction and destruction
    // ----------------------------------------------------------------------

    //! Construct TraceFileLogger object
    TraceFileLogger(const char* const compName  //!< The component name
    );
    
    //!  \brief Component initialization routine
    //!
    //!  The initialization function calls the initialization
    //!  routine for the base class.
    //!
    //!  \param queueDepth the depth of the message queue for the component
    //!  \param instance: instance identifier. Default: 0.
    void init(NATIVE_INT_TYPE queueDepth, NATIVE_INT_TYPE instance = 0);
    
    //!  \brief Set log file and max size
    //!
    //!  This is to create a log file to write all the trace buffers to.
    //!  The file will not be written to once the max size is hit.
    //!
    //!  \param fileName The name of the file to create.  Must be less than 80 characters.
    //!  \param maxSize The max size of the file
    //!
    //!  \return true if creating the file was successful, false otherwise
    void set_log_file(const char* fileName, const U32 maxSize);
    
    //!  \brief Trace Logger configure method
    //!
    //!  The configure method stores the file name to log traces.
    //!
    //!  \param file file where traces are stored.
    void configure(const char* file, const U32 maxSize);

    //!  \brief Trace Logger filter method
    //!
    //!  The filter method selects which trace types to be logged and which to ignore.
    //!
    //!  \param traceType_bitmask provides bitmask for trace types to select.
    //!  \param enable to turn on/off filtering .
    void filter(U16 traceType_bitmask,bool enable);

    //!  \brief Process trace ID storage method
    //!
    //!  The process trace ID storage lets you add/remove trace IDs from the array
    //!  that is used to evaluate which trace IDs to be ignored while logging 
    //!
    //!  \param traceId which trace id to enable / disable.
    //!  \param enable to turn on/off filtering .
    void process_traceId_storage(U32 traceId, bool enable);


    //! Destroy TraceFileLogger object
    ~TraceFileLogger();

  PRIVATE:
    // ----------------------------------------------------------------------
    // Handler implementations for user-defined typed input ports
    // ----------------------------------------------------------------------

    //! Handler implementation for TraceBufferLogger
    //!
    //! Logging Port
    //! Input Trace port to write to file
    void TraceBufferLogger_handler(FwIndexType portNum,        //!< The port number
                                   FwTraceIdType id,           //!< Trace ID
                                   Fw::Time& timeTag,          //!< Time Tag
                                   const Fw::TraceCfg::TraceType& type,  //!< The trace type argument
                                   Fw::TraceBuffer& args       //!< Buffer containing serialized trace entry
                                   ) override;

  PRIVATE:
    // ----------------------------------------------------------------------
    // Handler implementations for commands
    // ----------------------------------------------------------------------

    //! Handler implementation for command EnableTrace
    //!
    //! Enable or disable trace
    void EnableTrace_cmdHandler(FwOpcodeType opCode,  //!< The opcode
                                U32 cmdSeq,            //!< The command sequence number
                                Svc::TraceFileLogger_Enable enable) override;

    //! Handler implementation for command DumpTraceDp
    //!
    //! Dump Trace to a data product
    void DumpTraceDp_cmdHandler(FwOpcodeType opCode,  //!< The opcode
                                U32 cmdSeq            //!< The command sequence number
                                ) override;
   
    //! Handler implementation for command FilterTrace
    //!
    //! Select which trace types to be logged
    void FilterTrace_cmdHandler(FwOpcodeType opCode,            //!< The opcode
                                U32 cmdSeq,                     //!< The command sequence number
                                U16 bitmask,                    //!< TraceTypes to log on
                                Svc::TraceFileLogger_Enable enable  //!< enable or disable logging 
                                ) override;

    //! Handler implementation for command DisableTraceId
    //!
    //! Enable or disable trace logging by id, can disable up to 10 IDs
    void DisableTraceId_cmdHandler(FwOpcodeType opCode,  //!< The opcode
                                   U32 cmdSeq,           //!< The command sequence number
                                   U32 traceId,          //!< Trace ID to enable/disable
                                   Svc::TraceFileLogger_Enable enable) override;
    // ----------------------------------------------------------------------
    // Member Variables
    // ----------------------------------------------------------------------

    enum FileMode {
        CLOSED = 0,
        OPEN = 1
    };

    // The filename data:
    Os::File m_log_file; //Log file
    FileMode m_mode;    // file mode
    Fw::String m_fileName; //File name
    U32 m_maxFileSize; //max file size
    U32 m_byteCount; //current byte count of the file
    bool m_log_init; //Is logfile initialized
    bool m_enable_trace; //Is trace logging enabled
    U8 m_file_data[FW_TRACE_MAX_SER_SIZE]; //Holds a max size including metadata
    Fw::Buffer m_file_buffer;
    //filter trace types
    U16 m_traceFilter; //Select which trace types to allow logging
    //Array to filter on traceIds
    U32 traceId_array[FILTER_TRACE_ID_SIZE] = {0};
    ArrayProc filterTraceId;

    // ----------------------------------------------------------------------
    // File functions:
    // ----------------------------------------------------------------------
    void openFile(
    );

    void closeFile(
    );

    void write_log_file(
    U8* data,
    U32 size
    );

    };

}  // namespace Svc

#endif
