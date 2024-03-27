// ======================================================================
// \title  Random.hpp
// \author Rob Bocchino
// \brief  Random scenario
//
// \copyright
// Copyright (C) 2023 California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government sponsorship
// acknowledged.
// ======================================================================

#ifndef Svc_Random_HPP
#define Svc_Random_HPP

#include "Svc/DpManager/test/ut/TestState/TestState.hpp"

namespace Svc {

namespace Scenarios {

namespace Random {

class Tester {
  public:
    // ----------------------------------------------------------------------
    // Tests
    // ----------------------------------------------------------------------

    //! Run the random scenario
    void run(FwSizeType maxNumSteps  //!< The maximum number of steps
    );

  private:
    // ----------------------------------------------------------------------
    // Private member variables
    // ----------------------------------------------------------------------

    //! Test state
    TestState testState;
};

}  // namespace Random

}  // namespace Scenarios

}  // namespace Svc

#endif
