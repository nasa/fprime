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
#include <string>

#ifndef FPRIME_FAKELOGGER_HPP
#define FPRIME_FAKELOGGER_HPP
namespace MockLogging {
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
     * @param message: formatted message to log
     * @param size: size of message
     */
    void write(const char* message, FwSizeType size);

    /**
     * Check last message.
     * @param message: formatted message to check
     * @param size: size to log
     */
    virtual void check(const char* message);

    //!< Reset this logger
    void reset();

    //!< Last message that came in
    std::string m_last;
    //!< Logger to use within the system
    static Fw::Logger* s_current;
};
};      // namespace MockLogging
#endif  // FPRIME_FAKELOGGER_HPP
