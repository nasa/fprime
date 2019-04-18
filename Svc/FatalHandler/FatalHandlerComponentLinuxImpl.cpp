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

#include <stdlib.h>
#include <signal.h>
#include <Os/Log.hpp>
#include <Svc/FatalHandler/FatalHandlerComponentImpl.hpp>
#include "Fw/Types/BasicTypes.hpp"

namespace Svc {

    // ----------------------------------------------------------------------
    // Handler implementations for user-defined typed input ports
    // ----------------------------------------------------------------------

    void FatalHandlerComponentImpl::FatalReceive_handler(
            const NATIVE_INT_TYPE portNum,
            FwEventIdType Id) {
        // for **nix, delay then exit with error code
        Os::Log::logMsg("FATAL %d handled.\n",(U32)Id,0,0,0,0,0);
        (void)Os::Task::delay(1000);
        Os::Log::logMsg("Exiting.\n",0,0,0,0,0,0);
        (void)raise( SIGSEGV );
        exit(1);
    }

} // end namespace Svc
