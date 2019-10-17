// ====================================================================== 
// \title  Random.hpp
// \author bocchino
// \brief  Random number generation
//
// \copyright
// Copyright (C) 2017 California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
// ====================================================================== 

#ifndef STest_Random_HPP
#define STest_Random_HPP

#include <limits.h>
#include <sys/time.h>

#include "STest/types/basic_types.h"

namespace STest {

  namespace Random {

    enum {
      //! The maximum value of a random number
      MAX_VALUE = INT_MAX
    };

    namespace SeedValue {

      //! Get a seed value from the system time
      U32 getFromTime(void);

      //! Get a seed value from file
      //! \return true on success, false on failure
      bool getFromFile(
          const char *const fileName, //!< The file name
          U32& value //!< The seed value
      );

      //! Set the seed value
      void set(
          const U32 value //!< The seed value
      );

      //! Append a seed value to a file
      bool appendToFile(
          const char *const fileName, //!< The file name
          const U32 seedValue //!< The seed value
      );

    }

    //! Seed the random number generator:
    //! 1. Get the seed value from the file "seed" if possible, otherwise
    //!    from the current time.
    //! 2. Set the seed value.
    //! 3. Append the seed value to the file "seed-history"
    void seed(void);

    //! Return a random number in the range given by [start, start + length - 1].
    //! For example, Random::startLength(5, 3) returns a number
    //! between 5 and 7, inclusive.
    //! \return The number
    U32 startLength(
        const U32 start, //!< The start value of the range
        const U32 length //!< the length of the range, including the start and end values
    );

    //! Return a random number between the lower and the upper bound.
    //! For example, Random::lowerUpper(3, 5) returns a number
    //! between 3 and 5, inclusive.
    //! \return The number
    U32 lowerUpper(
        const U32 lower, //!< The lower bound
        const U32 upper //!< The upper bound
    );

    //! Return a random number in the interval [0, 1]
    double inUnitInterval(void);

  }

}

#endif
