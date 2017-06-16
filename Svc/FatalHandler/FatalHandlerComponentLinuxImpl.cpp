// ====================================================================== 
// \title  FatalHandlerImpl.cpp
// \author tcanham
// \brief  cpp file for FatalHandler component implementation class
//
// \copyright
// Copyright 2009-2015, by the California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged. Any commercial use must be negotiated with the Office
// of Technology Transfer at the California Institute of Technology.
// 
// This software may be subject to U.S. export control laws and
// regulations.  By accepting this document, the user agrees to comply
// with all U.S. export laws and regulations.  User has the
// responsibility to obtain export licenses, or other export authority
// as may be required before exporting such information to foreign
// countries or providing access to foreign persons.
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
