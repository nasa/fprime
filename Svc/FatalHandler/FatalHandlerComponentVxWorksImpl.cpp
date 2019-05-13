// ====================================================================== 
// \title  FatalHandlerImpl.cpp
// \author tcanham
// \brief  cpp file for FatalHandler component implementation class
//
// \copyright
// Copyright 2009-2015, by the California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
// 
// ====================================================================== 

#include <Svc/FatalHandler/FatalHandlerComponentImpl.hpp>
#include "Fw/Types/BasicTypes.hpp"
#include <taskLib.h>
#include <Os/Log.hpp>

namespace Svc {

    void FatalHandlerComponentImpl::FatalReceive_handler(
            const NATIVE_INT_TYPE portNum,
            FwEventIdType Id) {
        Os::Log::logMsg("FATAL %d handled.\n",(U32)Id,0,0,0,0,0);
        taskSuspend(0);
    }

} // end namespace Svc
