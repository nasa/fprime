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
#include "DeframerTester.hpp"

namespace Svc {

    //! Generate frames to send
    struct GenerateFrames : public STest::Rule<DeframerTester> {

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
            const DeframerTester& state //!< The test state
        );

        //! Action
        void action(
            DeframerTester& state //!< The test state
        );

    };

}

#endif
