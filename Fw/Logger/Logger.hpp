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
#include <Fw/Deprecate.hpp>
#include <FpConfig.hpp>
#include <Fw/Types/StringBase.hpp>

namespace Fw {
    class Logger {
        public:
            //! \brief log a formated string with supplied arguments
            //!
            //! Logs a format string with the arguments filled-in. This delegates to StringBase.format, which in-turn
            //! delegates to snprintf. This implies that the caller is fully responsible for handling the type safety of
            //! the supplied format string. The format string uses C-style (printf function family) formatting.
            //! \param format: format string
            //! \param ...: var-args list of arguments to inject into format string.
            void log(const char* format, ...);

            //! \brief log a string message directly
            //!
            //! Logs the string directly to the backing store without any formatting changes.
            //! \param message: message to log
            void log(const Fw::StringBase& message);

            /**
             * Logs a message using the currently specified static logger. If a logger is not
             * registered, then the log message is dropped.
             * \param fmt: format string in which to place arguments
             * \param a0: zeroth argument. (Default: 0)
             * \param a1: first argument. (Default: 0)
             * \param a2: second argument. (Default: 0)
             * \param a3: third argument. (Default: 0)
             * \param a4: fourth argument. (Default: 0)
             * \param a5: fifth argument. (Default: 0)
             * \param a6: sixth argument. (Default: 0)
             * \param a7: seventh argument. (Default: 0)
             * \param a8: eighth argument. (Default: 0)
             * \param a9: ninth argument. (Default: 0)
             */
            DEPRECATED(static void logMsg(
                const char* fmt,
                POINTER_CAST a0 = 0,
                POINTER_CAST a1 = 0,
                POINTER_CAST a2 = 0,
                POINTER_CAST a3 = 0,
                POINTER_CAST a4 = 0,
                POINTER_CAST a5 = 0,
                POINTER_CAST a6 = 0,
                POINTER_CAST a7 = 0,
                POINTER_CAST a8 = 0,
                POINTER_CAST a9 = 0
            ), "This function is deemed unsafe due to type-safety violations");

             //! \brief register a logger implementation
             //!
             //! This registers the supplied logger as the system logger used for calls to Fw::Logger::log.
             //! \param logger: logger to register as the system logger
            static void registerLogger(Logger* logger);

            //! Virtual destructor
            virtual ~Logger();

      protected:
        //! \brief write the output of the log message
        //!
        //! Log implementations must provide this method used to write the output of a string to the log backing.
        //!
        //! \param string: string to write to the log
        //! \param size: size of the string's contents to write to log
        virtual void write(const CHAR* string, const FwSizeType size) = 0;

      private:
        static Logger* s_current_logger; //!< Static logger to use when calling Fw::Logger::log function
    };
}

#endif
