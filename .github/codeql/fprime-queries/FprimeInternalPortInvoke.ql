/**
 * @name Internal port handler called directly
 * @description Internal port interactions must go through the auto-coded
 *              internal-interface invoke function, which enqueues the message
 *              on the component queue; calling the handler directly bypasses
 *              the component's threading model.
 * @kind problem
 * @id cpp/fprime/internal-port-invoke
 * @problem.severity warning
 * @tags correctness
 *       reliability
 *       external/jpl
 */

import cpp

from FunctionCall fc, Function caller
where
  fc.fromSource() and
  fc.getTarget().getName().matches("%\\_internalInterfaceHandler") and
  caller = fc.getEnclosingFunction() and
  // The auto-coded message dispatch is the only legitimate caller
  not caller.getName().toLowerCase().matches("%dispatch%")
select fc,
  "Internal port handler " + fc.getTarget().getName() +
    " is called directly; use the auto-coded " +
    fc.getTarget().getName().replaceAll("_internalInterfaceHandler", "_internalInterfaceInvoke") +
    " instead."
