/**
 * FakeLogger.hpp:
 *
 * Setup a fake logger for use with the testing. This allows for the capture of messages from the system and ensure that
 * the proper log messages are coming through as expected.
 *
 * @author mstarch
 */

#include <FpConfig.hpp>
#include <Fw/Logger/Logger.hpp>

#ifndef FPRIME_FAKELOGGER_HPP
#define FPRIME_FAKELOGGER_HPP
namespace MockLogging {
    /**
     * LogMessage data type to map inputs too.
     */
    struct LogMessage {
        const char *fmt;
        POINTER_CAST a0;
        POINTER_CAST a1;
        POINTER_CAST a2;
        POINTER_CAST a3;
        POINTER_CAST a4;
        POINTER_CAST a5;
        POINTER_CAST a6;
        POINTER_CAST a7;
        POINTER_CAST a8;
        POINTER_CAST a9;
    };
    /**
     * Fake logger used for two purposes:
     *   1. it acts as logging truth for the test
     *   2. it intercepts logging calls bound for the system
     */
    class FakeLogger : public Fw::Logger {
        public:
            //!< Constructor
            FakeLogger();

            /**
             * Fake implementation of the logger.
             * @param fmt: format
             * @param a0: arg0
             * @param a1: arg1
             * @param a2: arg2
             * @param a3: arg3
             * @param a4: arg4
             * @param a5: arg5
             * @param a6: arg6
             * @param a7: arg7
             * @param a8: arg8
             * @param a9: arg9
             */
            void log(
                    const char *fmt,
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
            );

            /**
             * Check last message.
             * @param fmt: format
             * @param a0: arg1
             * @param a1: arg1
             * @param a2: arg2
             * @param a3: arg3
             * @param a4: arg4
             * @param a5: arg5
             * @param a6: arg6
             * @param a7: arg6
             * @param a8: arg6
             * @param a9: arg6
             */
            virtual void check(
                    const char *fmt,
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
            );

            //!< Reset this logger
            void reset();

            //!< Last message that came in
            LogMessage m_last;
            //!< Logger to use within the system
            static Fw::Logger* s_current;
    };
};
#endif //FPRIME_FAKELOGGER_HPP
