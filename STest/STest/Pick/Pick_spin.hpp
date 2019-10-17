// ====================================================================== 
// \title  Pick_spin.hpp
// \author AUTO-GENERATED: DO NOT EDIT
// \brief  Pick_spin interface
//
// \copyright
// Copyright (C) 2019 California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
// ======================================================================

#ifndef STEST_PICK_SPIN_HPP
#define STEST_PICK_SPIN_HPP

#include <assert.h>
#include <cstddef>
#include <limits.h>

#include "STest/types/basic_types.h"

namespace STest {

  namespace Pick {

    namespace Spin {

      enum Constants {
        //! The number of random seeds to use
        NUM_RANDOM_SEEDS = 5,
        //! The number of random values to use
        NUM_RANDOM_VALUES = 5
      };

      //! Initialize the module
      void init(void);

      //! Initialize a rule
      void initRule(
          const U32 seedIndex //!< The index into the random seeds
      );

    }

  }

}

#endif
