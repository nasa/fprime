/**
 * @name Assertion on command or parameter input
 * @description FW_ASSERT must not be used to validate data that originates
 *              outside the software: command arguments and parameter values
 *              come from the ground and can be wrong without the software
 *              being wrong. Validate such input and respond with a command
 *              status or event instead.
 * @kind problem
 * @id cpp/fprime/assert-on-untrusted-input
 * @problem.severity error
 * @tags correctness
 *       reliability
 *       external/jpl
 */

import cpp
import semmle.code.cpp.dataflow.new.DataFlow
import FprimeAssertions

/** A source of ground-provided data. */
predicate untrustedSource(DataFlow::Node src, string what) {
  exists(Parameter p, Function f |
    src = DataFlow::parameterNode(p) and
    f = p.getFunction() and
    f.getName().matches("%\\_cmdHandler") and
    // opCode and cmdSeq are framework bookkeeping, not ground data
    p.getIndex() >= 2 and
    what = "command argument " + p.getName()
  )
  or
  exists(FunctionCall pg |
    src.asExpr() = pg and
    pg.getTarget().getName().matches("paramGet\\_%") and
    what = "parameter value from " + pg.getTarget().getName()
  )
}

from FwAssert a, DataFlow::Node src, DataFlow::Node sink, string what
where
  a.getAsserted().getAChild*() = sink.asExpr() and
  untrustedSource(src, what) and
  DataFlow::localFlow(src, sink) and
  not a.getFile().getAbsolutePath().matches(["%/test/%", "%/ut/%"])
select a, "FW_ASSERT checks " + what + ", which originates from the ground; validate and report instead."
