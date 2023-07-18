// ======================================================================
// \title  Pick.hpp
// \author bocchino
// \brief  Pick interface
//
// \copyright
// Copyright (C) 2022 California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
// ======================================================================

#ifndef STEST_PICK_HPP
#define STEST_PICK_HPP

#include "STest/types/basic_types.h"
#ifdef STEST_MODE_spin
#include "STest/Pick/Pick_spin.hpp"
#endif

namespace STest {

  namespace Pick {

    //! Pick a double value in the interval [0, 1]
    double inUnitInterval();

    //! Return a U32 value in the range given by [start, start + length - 1].
    //! For example, startLength(5, 3) returns a number
    //! between 5 and 7, inclusive.
    //! \return The U32 value
    U32 startLength(
        const U32 start, //!< The start value of the range
        const U32 length //!< The length of the range, including the start and end values
    );

    //! Return a U32 value between the lower and the upper bound.
    //! For example, lowerUpper(3, 5) returns a number between 3 and 5,
    //! inclusive.
    //! \return The U32 value
    U32 lowerUpper(
        const U32 lower, //!< The lower bound
        const U32 upper //!< The upper bound
    );

    //! Return an arbitrary U32 value
    //! \return The U32 value
    U32 any();

  }

}

#endif
