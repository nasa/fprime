/**
 * File: Logger.hpp
 * Description: Framework logging support
 * Author: mstarch
 *
 * This file adds in support to the core 'Fw' package, to separate it from Os and other loggers, and
 * allow the architect of the system to select which core framework logging should be used.
 */
#ifndef Fw_Logger_hpp_
#define Fw_Logger_hpp_
#include <FpConfig.hpp>
#include <Fw/Deprecate.hpp>
#include <Fw/Types/StringBase.hpp>

// Unit testing predeclaration hook
namespace LoggerRules {
struct Register;
}

namespace Fw {
class Logger {
    friend struct LoggerRules::Register;

  public:
    //! \brief log a formated string with supplied arguments
    //!
    //! Logs a format string with the arguments filled-in. This delegates to StringBase.format, which in-turn
    //! delegates to snprintf. This implies that the caller is fully responsible for handling the type safety of
    //! the supplied format string. The format string uses C-style (printf function family) formatting.
    //! \param format: format string
    //! \param ...: var-args list of arguments to inject into format string.
    static void log(const char* format, ...);

    //! \brief log a string message directly
    //!
    //! Logs the string directly to the backing store without any formatting changes.
    //! \param message: message to log
    static void log(const Fw::StringBase& message);

    //! \brief register a logger implementation
    //!
    //! This registers the supplied logger as the system logger used for calls to Fw::Logger::log.
    //! \param logger: logger to register as the system logger
    static void registerLogger(Logger* logger);

    //! Virtual destructor
    virtual ~Logger() = default;

  protected:
    //! \brief write the output of the log message
    //!
    //! Log implementations must provide this method used to write the output of a string to the log backing.
    //!
    //! \param message: message to log
    virtual void writeMessage(const StringBase& message) = 0;

  private:
    static Logger* s_current_logger;  //!< Static logger to use when calling Fw::Logger::log function
};
}  // namespace Fw

#endif
