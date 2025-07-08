// ======================================================================
// \title  ActivePhaserTester.cpp
// \author mstarch
// \brief  cpp file for ActivePhaser component test harness implementation class
//
// \copyright
// Copyright 2009-2015, by the California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
//
// ======================================================================

#include "ActivePhaserTester.hpp"

namespace Svc {

// ----------------------------------------------------------------------
// Construction and destruction
// ----------------------------------------------------------------------

ActivePhaserTester ::ActivePhaserTester()
    : ActivePhaserGTestBase("ActivePhaserTester", ActivePhaserTester::MAX_HISTORY_SIZE), component("ActivePhaser") {
    this->initComponents();
    this->connectPorts();
}

ActivePhaserTester ::~ActivePhaserTester() {}

// ----------------------------------------------------------------------
// Tests
// ----------------------------------------------------------------------

void ActivePhaserTester ::toDo() {
    // TODO
}

}  // namespace Svc
