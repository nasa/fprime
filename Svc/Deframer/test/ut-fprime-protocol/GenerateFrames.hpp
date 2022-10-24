//! ======================================================================
//! \title DeframerRules.hpp
//! \brief Header file for GenerateFrames rule
//! \author lestarch, bocchino
//! ======================================================================

#ifndef SVC_GENERATE_FRAMES_HPP
#define SVC_GENERATE_FRAMES_HPP

#include <FpConfig.hpp>
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
        bool precondition(
            const Tester& state //!< The test state
        );

        //! Action
        void action(
            Tester& state //!< The test state
        );

    };

}

#endif
