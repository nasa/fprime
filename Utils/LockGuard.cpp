// ======================================================================
// \title  LockGuard.cpp
// \author vwong
// \brief  cpp file for a lock guard utility class
//
// 
// \copyright
// Copyright (C) 2009-2020 California Institute of Technology.
// ALL RIGHTS RESERVED. United States Government Sponsorship
// acknowledged.
// ====================================================================== 

#include "Utils/LockGuard.hpp"

namespace Utils {

  LockGuard ::
    LockGuard (
        Os::Mutex& mutex
    ) :
      m_mutex(mutex)
  {
    this->m_mutex.lock();
  }

  LockGuard ::
    ~LockGuard ()
  {
    this->m_mutex.unLock();
  }

} // end namespace Utils
