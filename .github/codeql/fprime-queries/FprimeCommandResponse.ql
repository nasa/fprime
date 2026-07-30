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
 * Holds if `call` sends a command response with `opExpr`/`seqExpr` as opcode
 * and sequence: a `cmdResponse_out` call, or a call to a helper that forwards
 * two of its parameters to a (transitive) `cmdResponse_out` call.
 */
predicate responseCall(FunctionCall call, Expr opExpr, Expr seqExpr) {
  call instanceof CmdResponseCall and
  opExpr = call.getArgument(0) and
  seqExpr = call.getArgument(1)
  or
  exists(Function g, Parameter gOp, Parameter gSeq, FunctionCall inner |
    call.getTarget() = g and
    gOp = g.getAParameter() and
    gSeq = g.getAParameter() and
    inner.getEnclosingFunction() = g and
    responseCall(inner, gOp.getAnAccess(), gSeq.getAnAccess()) and
    opExpr = call.getArgument(gOp.getIndex()) and
    seqExpr = call.getArgument(gSeq.getIndex())
  )
}

/**
 * Holds if `node` is a call inside `h` that sends the command response
 * directly (possibly via a helper), reading both the opcode and sequence
 * parameters. The response value is not constrained: sending the wrong value
 * is a different defect than never responding at all.
 */
predicate directResponse(CmdHandler h, ControlFlowNode node) {
  exists(FunctionCall call | call = node |
    call.getEnclosingFunction() = h and
    (
      call instanceof CmdResponseCall
      or
      responseCall(call, h.getOpCodeParameter().getAnAccess(), h.getCmdSeqParameter().getAnAccess())
    )
  )
}

/**
 * Holds if `node` stores `valueExpr` into member variable `f`: a direct
 * assignment, or a call to a helper that assigns one of its parameters to `f`.
 */
predicate fieldStore(ControlFlowNode node, Expr valueExpr, Field f) {
  exists(Assignment a | a = node |
    a.getRValue() = valueExpr and
    a.getLValue().(FieldAccess).getTarget() = f
  )
  or
  exists(FunctionCall call, Function g, Parameter gP | call = node |
    call.getTarget() = g and
    gP = g.getAParameter() and
    exists(ControlFlowNode inner |
      inner.(Expr).getEnclosingFunction() = g and
      fieldStore(inner, gP.getAnAccess(), f)
    ) and
    valueExpr = call.getArgument(gP.getIndex())
  )
}

/**
 * Holds if `node`, inside `h`, stores parameter `p` into member variable `f`
 * of the handler's component class (directly or via a base class).
 */
predicate memberStore(CmdHandler h, Parameter p, Field f, ControlFlowNode node) {
  node.getControlFlowScope() = h and
  fieldStore(node, p.getAnAccess(), f) and
  h.getDeclaringType().getABaseClass*() = f.getDeclaringType()
}

/**
 * Holds if some response call reads member variables `fOp` and `fSeq` as its
 * opcode and sequence arguments, completing a deferred command response.
 */
predicate deferredResponseRead(Field fOp, Field fSeq) {
  exists(Expr opExpr, Expr seqExpr |
    responseCall(_, opExpr, seqExpr) and
    opExpr.(FieldAccess).getTarget() = fOp and
    seqExpr.(FieldAccess).getTarget() = fSeq
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
