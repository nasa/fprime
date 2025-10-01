/*
 * EventManagerTester.hpp
 *
 *  Created on: Mar 18, 2015
 *      Author: tcanham
 */

#ifndef EventManager_TEST_UT_EventManager_TESTER_HPP_
#define EventManager_TEST_UT_EventManager_TESTER_HPP_

#include <EventManagerGTestBase.hpp>
#include <Os/File.hpp>
#include <Svc/EventManager/EventManager.hpp>

namespace Svc {

class EventManagerTester : public Svc::EventManagerGTestBase {
  public:
    explicit EventManagerTester(Svc::EventManager& inst);
    virtual ~EventManagerTester();

    void runEventNominal();
    void runFilterEventNominal();
    void runFilterIdNominal();
    void runFilterDump();
    void runFilterInvalidCommands();
    void runEventFatal();
    void runFileDump();
    void runFileDumpErrors();

  private:
    void from_PktSend_handler(const FwIndexType portNum,  //!< The port number
                              Fw::ComBuffer& data,        //!< Buffer containing packet data
                              U32 context                 //!< context (not used)
                              ) override;
    void from_FatalAnnounce_handler(const FwIndexType portNum,  //!< The port number
                                    FwEventIdType Id            //!< The ID of the FATAL event
                                    ) override;

    Svc::EventManager& m_impl;

    bool m_receivedPacket;
    Fw::ComBuffer m_sentPacket;

    bool m_receivedFatalEvent;
    FwEventIdType m_fatalID;

    void runWithFilters(Fw::LogSeverity filter);

    void writeEvent(FwEventIdType id, Fw::LogSeverity severity, U32 value);
    void readEvent(FwEventIdType id, Fw::LogSeverity severity, U32 value, Os::File& file);

    // enumeration to tell what kind of error to inject
    typedef enum {
        FILE_WRITE_WRITE_ERROR,  // return a bad read status
        FILE_WRITE_SIZE_ERROR,   // return a bad size
    } FileWriteTestType;
    FileWriteTestType m_writeTestType;
    FwSizeType m_writeSize;

    void textLogIn(const FwEventIdType id,          //!< The event ID
                   const Fw::Time& timeTag,         //!< The time
                   const Fw::LogSeverity severity,  //!< The severity
                   const Fw::TextLogString& text    //!< The event string
                   ) override;

    //! Handler for from_pingOut
    //!
    void from_pingOut_handler(const FwIndexType portNum, /*!< The port number*/
                              U32 key                    /*!< Value to return to pinger*/
                              ) override;
};

} /* namespace Svc */

#endif /* EventManager_TEST_UT_EventManager_TESTER_HPP_ */
