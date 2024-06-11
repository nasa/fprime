// ====================================================================== 
// \title  LockGuard.hpp
// \author vwong
// \brief  hpp file for a RAII-style lock guard utility class
//
// \copyright
// Copyright (C) 2009-2020 California Institute of Technology.
// ALL RIGHTS RESERVED. United States Government Sponsorship
// acknowledged.
// ====================================================================== 

#ifndef LockGuard_HPP
#define LockGuard_HPP

#include <FpConfig.hpp>
#include <Os/Mutex.hpp>

namespace Utils {

  class LockGuard
  {

    public:

      // Construct and lock associated mutex
      LockGuard(Os::Mutex& mutex);

      // Destruct and unlock associated mutex
      ~LockGuard();

    private:

      // parameters
      Os::Mutex& m_mutex;
  };

} // end namespace Utils

#endif
