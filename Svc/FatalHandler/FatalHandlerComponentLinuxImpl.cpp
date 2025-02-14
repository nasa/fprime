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

#include <cstdlib>
#include <csignal>
#include <Fw/Logger/Logger.hpp>
#include <Svc/FatalHandler/FatalHandlerComponentImpl.hpp>
#include <Os/Task.hpp>
#include <FpConfig.hpp>

namespace Svc {

    // ----------------------------------------------------------------------
    // Handler implementations for user-defined typed input ports
    // ----------------------------------------------------------------------

    void FatalHandlerComponentImpl::FatalReceive_handler(
            const FwIndexType portNum,
            FwEventIdType Id) {
        // for **nix, delay then exit with error code
        Fw::Logger::log("FATAL %d handled.\n",Id);
        (void)Os::Task::delay(Fw::TimeInterval(1, 0));
        Fw::Logger::log("Exiting with abort signal and core dump file.\n");
        (void)raise( SIGABRT );
        exit(1);
    }

} // end namespace Svc
