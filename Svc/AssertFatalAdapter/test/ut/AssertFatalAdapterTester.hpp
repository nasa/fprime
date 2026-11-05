// ======================================================================
// \title  AssertFatalAdapter/test/ut/Tester.hpp
// \author tcanham
// \brief  hpp file for AssertFatalAdapter test harness implementation class
//
// \copyright
// Copyright 2009-2015, by the California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
//
// ======================================================================

#ifndef TESTER_HPP
#define TESTER_HPP

#include <string>
#include <vector>
#include "AssertFatalAdapterGTestBase.hpp"
#include "Svc/AssertFatalAdapter/AssertFatalAdapterComponentImpl.hpp"

namespace Svc {

class AssertFatalAdapterTester : public AssertFatalAdapterGTestBase {
    // ----------------------------------------------------------------------
    // Construction and destruction
    // ----------------------------------------------------------------------

  public:
    //! Construct object AssertFatalAdapterTester
    //!
    AssertFatalAdapterTester();

    //! Destroy object AssertFatalAdapterTester
    //!
    ~AssertFatalAdapterTester();

  public:
    // ----------------------------------------------------------------------
    // Tests
    // ----------------------------------------------------------------------

    //! To do
    //!
    void testAsserts();

    //! Test file path truncation
    //!
    void testTruncation();

  private:
    // ----------------------------------------------------------------------
    // Helper methods
    // ----------------------------------------------------------------------

    //! Connect ports
    //!
    void connectPorts();

    //! Initialize components
    //!
    void initComponents();

  private:
    // ----------------------------------------------------------------------
    // Variables
    // ----------------------------------------------------------------------

    //! The component under test
    //!
    AssertFatalAdapterComponentImpl component;

    //! Storage for text log messages
    //!
    std::vector<std::string> textLogMessages;

    void textLogIn(const FwEventIdType id,          //!< The event ID
                   const Fw::Time& timeTag,         //!< The time
                   const Fw::LogSeverity severity,  //!< The severity
                   const Fw::TextLogString& text    //!< The event string
                   ) override;

    //! Clear text log history
    //!
    void clearTextLogs();

    //! Get the last text log message
    //!
    const std::string& getLastTextLog() const;

    //! Fill buffer with identifiable byte pattern for debugging
    //! Writes repeating pattern: "00112233...99" to make byte positions visible
    //! \param buffer: Character buffer to fill
    //! \param size: Total size of buffer (includes null terminator)
    //! \param nullTerminate: If true, writes null terminator at end
    //!
    static void fillWithBytePattern(char* buffer, FwSizeType size, bool nullTerminate = true);
};

}  // end namespace Svc

#endif
