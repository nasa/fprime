// ======================================================================
// \title  ComQueue/test/ut/Tester.hpp
// \author vbai
// \brief  hpp file for ComQueue test harness implementation class
// ======================================================================

#ifndef TESTER_HPP
#define TESTER_HPP

#include "GTestBase.hpp"
#include <Fw/Types/Assert.hpp>
#include <Fw/Test/UnitTest.hpp>
#include "Svc/ComQueue/ComQueue.hpp"


#define MAX_HISTORY_SIZE 10
#define BUFFER_CAPACITY 100
#define COM_BUFFER_CAPACITY 100


namespace Svc {

  class Tester :
    public ComQueueGTestBase
  {

    private:

      // ----------------------------------------------------------------------
      // Types
      // ----------------------------------------------------------------------

      enum class ThrottleState {
          THROTTLED,
          NOT_THROTTLED
      };

      // ----------------------------------------------------------------------
      // Construction and destruction
      // ----------------------------------------------------------------------

    public:

      //! Construct object Tester
      //!
      Tester();

      //! Destroy object Tester
      //!
      ~Tester();

    public:

      // ----------------------------------------------------------------------
      // Tests
      // ----------------------------------------------------------------------

      //! To do
      //!

      //! Test round robin, throtlle ie QueueFull
      void testThrottle();
      void testRoundRobin(); 

    private:

      // ----------------------------------------------------------------------
      // Handlers for typed from ports
      // ----------------------------------------------------------------------

      //! Handler for from_buffQueueSend
      //!
      void from_buffQueueSend_handler(
          const NATIVE_INT_TYPE portNum, /*!< The port number*/
          Fw::Buffer &fwBuffer
      );

      //! Handler for from_comQueueSend
      //!
      void from_comQueueSend_handler(
          const NATIVE_INT_TYPE portNum, /*!< The port number*/
          Fw::ComBuffer &data, /*!< Buffer containing packet data*/
          U32 context /*!< Call context value; meaning chosen by user*/
      );

    private:

      // ----------------------------------------------------------------------
      // Helper methods
      // ----------------------------------------------------------------------

      //     void retryQueue();
      //    void processQueue();

      //! Test the retry status
      //!
      void testRetryQueue();

      //! Test the queue processing
      void testProcessQueue();

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
      ComQueue component;

  };

} // end namespace Svc

#endif
