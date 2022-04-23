//! ======================================================================
//! DeframerRules.hpp
//! Header file for Deframer unit test rules
//! @author lestarch, bocchino
//! ======================================================================

#ifndef SVC_DEFRAMER_RULES
#define SVC_DEFRAMER_RULES

#include "Fw/Types/BasicTypes.hpp"
#include "Fw/Types/StringType.hpp"
#include "STest/STest/Pick/Pick.hpp"
#include "STest/STest/Rule/Rule.hpp"
#include "Svc/Deframer/test/ut-fprime-protocol/Tester.hpp"

namespace Svc {

    //! Generate frames to send
    struct GenerateFrames : public STest::Rule<Tester> {

        // ----------------------------------------------------------------------
        // Construction 
        // ----------------------------------------------------------------------

        //! Constructor
        GenerateFrames();

        // ----------------------------------------------------------------------
        // Public member functions 
        // ----------------------------------------------------------------------

        //! Precondition
        bool precondition(const Tester& state);

        //! Action
        void action(Tester& state);

    };

    //! Pack generated frames into a buffer
    //! Send the buffer
    class SendBuffer : public STest::Rule<Tester> {

      public:

        // ----------------------------------------------------------------------
        // Construction 
        // ----------------------------------------------------------------------

        //! Constructor
        SendBuffer();

      public:

        // ----------------------------------------------------------------------
        // Public functions 
        // ----------------------------------------------------------------------

        //! Precondition
        bool precondition(const Tester& state);

        //! Action
        void action(Svc::Tester &state);

      private:

        // ----------------------------------------------------------------------
        // Private helper functions 
        // ----------------------------------------------------------------------

        //! Fill the incoming buffer with frame data
        void fillIncomingBuffer(
            Svc::Tester &state //!< The test state
        );

        //! Record a received frame
        void recordReceivedFrame(
            Svc::Tester& state, //!< The test state
            Svc::Tester::UplinkFrame& frame //!< The frame
        );

      private:
        
        // ----------------------------------------------------------------------
        // Private member variables 
        // ----------------------------------------------------------------------

        //! The expected number of com buffers emitted
        U32 expectedComCount;

        //! The expected number of file packet buffers emitted
        U32 expectedBuffCount;

    };

};

#endif
