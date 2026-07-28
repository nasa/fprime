/**
 * @name Command handler missing command response
 * @description Every path through a command handler
 *              (`*_cmdHandler(FwOpcodeType opCode, U32 cmdSeq, ...)`) must
 *              either call `cmdResponse_out` directly, or store both `opCode`
 *              and `cmdSeq` into member variables that are later passed to a
 *              `cmdResponse_out` call (the deferred command response pattern).
 *              A handler with a path that does neither may leave the ground
 *              system waiting forever for a command completion.
 * @kind problem
 * @id cpp/fprime/command-response
 * @problem.severity warning
 * @tags correctness
 *       reliability
 */

import cpp

/** The declared type name of `p`, with any top-level `const` stripped. */
string parameterTypeName(Parameter p) {
  exists(Type t | t = p.getType() |
    result = t.getName() or result = t.(SpecifiedType).getBaseType().getName()
  )
}

/**
 * An implementation of an F Prime command handler: a member function named
 * `*cmdHandler` whose first two parameters are the opcode
 * (`FwOpcodeType`) and the command sequence number (`U32`).
 */
class CmdHandler extends MemberFunction {
  CmdHandler() {
    this.getName().regexpMatch(".*cmdHandler") and
    parameterTypeName(this.getParameter(0)) = "FwOpcodeType" and
    parameterTypeName(this.getParameter(1)) = "U32" and
    this.hasDefinition()
  }

  Parameter getOpCodeParameter() { result = this.getParameter(0) }

  Parameter getCmdSeqParameter() { result = this.getParameter(1) }
}

/** A call to the autocoded command response output port invocation function. */
class CmdResponseCall extends FunctionCall {
  CmdResponseCall() { this.getTarget().getName() = "cmdResponse_out" }
}

/**
 * Holds if `node` is a call inside `h` that sends the command response
 * directly, i.e. any `cmdResponse_out` call. The opcode/sequence arguments are
 * not constrained: passing the wrong values is a different defect than never
 * responding at all.
 */
predicate directResponse(CmdHandler h, ControlFlowNode node) {
  node.(CmdResponseCall).getEnclosingFunction() = h
}

/**
 * Holds if `node` is an assignment inside `h` that stores parameter `p` into
 * member variable `f` of the handler's component class (directly or via a
 * base class).
 */
predicate memberStore(CmdHandler h, Parameter p, Field f, ControlFlowNode node) {
  exists(Assignment a |
    a = node and
    a.getEnclosingFunction() = h and
    a.getRValue() = p.getAnAccess() and
    a.getLValue().(FieldAccess).getTarget() = f and
    h.getDeclaringType().getABaseClass*() = f.getDeclaringType()
  )
}

/**
 * Holds if some `cmdResponse_out` call reads member variables `fOp` and
 * `fSeq` as its opcode and sequence arguments, completing a deferred command
 * response.
 */
predicate deferredResponseRead(Field fOp, Field fSeq) {
  exists(CmdResponseCall c |
    c.getArgument(0).(FieldAccess).getTarget() = fOp and
    c.getArgument(1).(FieldAccess).getTarget() = fSeq
  )
}

/**
 * Holds if `h` contains stores of its opcode parameter into `fOp` and its
 * sequence parameter into `fSeq`, and some `cmdResponse_out` call later reads
 * both: the deferred command response pattern.
 */
predicate deferredPair(CmdHandler h, Field fOp, Field fSeq) {
  memberStore(h, h.getOpCodeParameter(), fOp, _) and
  memberStore(h, h.getCmdSeqParameter(), fSeq, _) and
  deferredResponseRead(fOp, fSeq)
}

/** Holds if `node` stores the opcode parameter of `h` for a deferred response. */
predicate deferredOpCodeStore(CmdHandler h, ControlFlowNode node) {
  exists(Field fOp | deferredPair(h, fOp, _) and memberStore(h, h.getOpCodeParameter(), fOp, node))
}

/** Holds if `node` stores the sequence parameter of `h` for a deferred response. */
predicate deferredCmdSeqStore(CmdHandler h, ControlFlowNode node) {
  exists(Field fSeq | deferredPair(h, _, fSeq) and memberStore(h, h.getCmdSeqParameter(), fSeq, node))
}

/**
 * Control-flow nodes of `h` reachable from its entry without passing through
 * a direct `cmdResponse_out` call or a deferred-response store of the opcode
 * parameter.
 */
predicate flowMissingOpCodeAction(CmdHandler h, ControlFlowNode n) {
  n = h.getBlock()
  or
  exists(ControlFlowNode mid |
    flowMissingOpCodeAction(h, mid) and
    not directResponse(h, mid) and
    not deferredOpCodeStore(h, mid) and
    n = mid.getASuccessor()
  )
}

/**
 * Control-flow nodes of `h` reachable from its entry without passing through
 * a direct `cmdResponse_out` call or a deferred-response store of the
 * sequence parameter.
 */
predicate flowMissingCmdSeqAction(CmdHandler h, ControlFlowNode n) {
  n = h.getBlock()
  or
  exists(ControlFlowNode mid |
    flowMissingCmdSeqAction(h, mid) and
    not directResponse(h, mid) and
    not deferredCmdSeqStore(h, mid) and
    n = mid.getASuccessor()
  )
}

/**
 * Holds if some path through `h` reaches the function exit without a direct
 * `cmdResponse_out` call and without completing a deferred response (a path
 * satisfies the deferred pattern only if it stores both the opcode and the
 * sequence number into member variables that a `cmdResponse_out` call reads).
 */
predicate hasUnrespondedPath(CmdHandler h) {
  flowMissingOpCodeAction(h, h) or
  flowMissingCmdSeqAction(h, h)
}

from CmdHandler h
where hasUnrespondedPath(h)
select h,
  "Command handler " + h.getName() +
    " has a path that neither calls cmdResponse_out nor saves opCode and cmdSeq" +
    " into member variables read by a deferred cmdResponse_out call."
