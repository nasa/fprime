/**
 * @name Unbounded loop
 * @description All loops should have a fixed upper bound; the counter should also be incremented along all paths within the loop.
 *              This check excludes loops that are meant to be nonterminating (like schedulers).
 * @kind problem
 * @id cpp/fprime/jpl-c/loop-bounds
 * @problem.severity warning
 * @tags correctness
 *       external/jpl
 */

import cpp

/*
 * F Prime refinement of the stock JPL Rule 3 query (cpp/jpl-c/loop-bounds).
 * A C++ range-based for statement over a std::initializer_list iterates a
 * compile-time-fixed sequence, so it is inherently bounded; the stock query
 * reports it because the desugared `__begin != __end` condition contains no
 * relational bound check. The F Prime autocoder emits this pattern in every
 * generated array class's initializer_list operator=.
 */
predicate isInitializerListRangeFor(Loop l) {
  l.(RangeBasedForStmt).getRange().getType().stripType().(Class).hasQualifiedName("std", "initializer_list")
}

predicate validVarForBound(Loop loop, Variable var) {
  // The variable is read in the loop controlling expression
  var.getAnAccess().getParent*() = loop.getControllingExpr() and
  // The variable is not assigned in the loop body
  not inScope(loop, var.getAnAssignment().getEnclosingStmt()) and
  // The variable is not incremented/decremented in the loop body
  not inScope(loop, var.getAnAccess().getParent().(CrementOperation).getEnclosingStmt())
}

predicate upperBoundCheck(Loop loop, VariableAccess checked) {
  exists(RelationalOperation rop | loop.getControllingExpr().getAChild*() = rop |
    checked = rop.getLesserOperand() and
    // The RHS is something "valid", i.e. a constant or
    // a variable that isn't assigned in the loop body
    (
      exists(rop.getGreaterOperand().getValue()) or
      rop.getGreaterOperand().(VariableAccess).getTarget().isConst() or
      validVarForBound(loop, rop.getGreaterOperand().(VariableAccess).getTarget())
    ) and
    not rop.getGreaterOperand() instanceof CharLiteral
  )
}

predicate lowerBoundCheck(Loop loop, VariableAccess checked) {
  exists(RelationalOperation rop | loop.getControllingExpr().getAChild*() = rop |
    checked = rop.getGreaterOperand() and
    // The RHS is something "valid", i.e. a constant or
    // a variable that isn't assigned in the loop body
    (
      exists(rop.getLesserOperand().getValue()) or
      rop.getLesserOperand().(VariableAccess).getTarget().isConst() or
      validVarForBound(loop, rop.getLesserOperand().(VariableAccess).getTarget())
    ) and
    not rop.getLesserOperand() instanceof CharLiteral
  )
}

VariableAccess getAnIncrement(Variable var) {
  result.getTarget() = var and
  (
    result.getParent() instanceof IncrementOperation
    or
    exists(AssignAddExpr a | a.getLValue() = result and a.getRValue().getValue().toInt() > 0)
    or
    exists(AssignExpr a | a.getLValue() = result |
      a.getRValue() =
        any(AddExpr ae |
          ae.getAnOperand() = var.getAnAccess() and
          ae.getAnOperand().getValue().toInt() > 0
        )
    )
  )
}

VariableAccess getADecrement(Variable var) {
  result.getTarget() = var and
  (
    result.getParent() instanceof DecrementOperation
    or
    exists(AssignSubExpr a | a.getLValue() = result and a.getRValue().getValue().toInt() > 0)
    or
    exists(AssignExpr a | a.getLValue() = result |
      a.getRValue() =
        any(SubExpr ae |
          ae.getLeftOperand() = var.getAnAccess() and
          ae.getRightOperand().getValue().toInt() > 0
        )
    )
  )
}

predicate inScope(Loop l, Stmt s) { l.getAChild*() = s }

predicate reachesNoInc(VariableAccess source, ControlFlowNode target) {
  upperBoundCheck(_, source) and source.getASuccessor() = target
  or
  exists(ControlFlowNode mid |
    reachesNoInc(source, mid) and not mid = getAnIncrement(source.getTarget())
  |
    target = mid.getASuccessor() and
    inScope(source.getEnclosingStmt(), target.getEnclosingStmt())
  )
}

predicate reachesNoDec(VariableAccess source, ControlFlowNode target) {
  lowerBoundCheck(_, source) and source.getASuccessor() = target
  or
  exists(ControlFlowNode mid |
    reachesNoDec(source, mid) and not mid = getADecrement(source.getTarget())
  |
    target = mid.getASuccessor() and
    inScope(source.getEnclosingStmt(), target.getEnclosingStmt())
  )
}

predicate hasSafeBound(Loop l) {
  exists(VariableAccess bound | upperBoundCheck(l, bound) | not reachesNoInc(bound, bound))
  or
  exists(VariableAccess bound | lowerBoundCheck(l, bound) | not reachesNoDec(bound, bound))
  or
  exists(l.getControllingExpr().getValue())
}

predicate markedAsNonterminating(Loop l) {
  exists(Comment c | c.getContents().matches("%@non-terminating@%") | c.getCommentedElement() = l)
}

from Loop loop, string msg
where
  not hasSafeBound(loop) and
  not markedAsNonterminating(loop) and
  not isInitializerListRangeFor(loop) and
  (
    not upperBoundCheck(loop, _) and
    not lowerBoundCheck(loop, _) and
    msg = "This loop does not have a fixed bound."
    or
    exists(VariableAccess bound |
      upperBoundCheck(loop, bound) and
      reachesNoInc(bound, bound) and
      msg =
        "The loop counter " + bound.getTarget().getName() +
          " is not always incremented in the loop body."
    )
    or
    exists(VariableAccess bound |
      lowerBoundCheck(loop, bound) and
      reachesNoDec(bound, bound) and
      msg =
        "The loop counter " + bound.getTarget().getName() +
          " is not always decremented in the loop body."
    )
  )
select loop, msg
