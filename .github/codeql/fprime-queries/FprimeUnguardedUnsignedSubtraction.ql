/**
 * @name Unguarded unsigned subtraction from a run-time size
 * @description Subtracting a compile-time constant from a size obtained at
 *              run time underflows to a very large value, rather than going
 *              negative, when the size is smaller than the constant. A check
 *              may be a condition or an FW_ASSERT; this reports sites with
 *              neither.
 * @kind problem
 * @id cpp/fprime/unguarded-unsigned-subtraction
 * @problem.severity warning
 * @tags correctness
 *       reliability
 *       external/cwe/cwe-191
 */

import cpp

/*
 * Motivated by nasa/fprime#5518, where `DpCompressProc::procRequest_handler`
 * computed `fwBuffer.getSize() - Fw::DpContainer::MIN_PACKET_SIZE` on a
 * port-supplied buffer with no check of any kind in the handler.
 *
 * Deliberately conservative, to be usable as a CI gate: the subtrahend must
 * be a compile-time constant, the minuend must come from a known size
 * accessor, and check recognition is direction-agnostic with no dominance
 * analysis. A bound of `C` is accepted for `accessor - K` when `C >= K - 1`,
 * so `if (size > 0)` credits `size - 1`; `test.cpp` pins that case.
 */

/**
 * A call returning a size or length determined at run time, rather than
 * one the calling function chose itself.
 */
predicate isSizeAccessor(Call c) {
  c.getTarget().getName() =
    [
      "getSize", "getBuffLength", "getDataSize", "getBuffCapacity",
      "getDeserializeSizeLeft", "getSerializeSizeLeft"
    ]
}

/**
 * Holds if `e` reads the size accessor `name`: either a direct call, or an
 * access to a local variable whose initializer is that call. The local-copy
 * form is idiomatic -- `const FwSizeType n = buf.getSize();` then a check on
 * `n` -- and a check written that way is still a check.
 */
predicate readsSize(Expr e, string name) {
  isSizeAccessor(e.(Call)) and name = e.(Call).getTarget().getName()
  or
  exists(LocalScopeVariable v, Call acc |
    e = v.getAnAccess() and
    acc = v.getInitializer().getExpr() and
    isSizeAccessor(acc) and
    name = acc.getTarget().getName()
  )
}

/**
 * Holds if `f` compares a read of the size accessor `name` against a bound
 * that is not demonstrably too small for `accessor - k`. An `FW_ASSERT`
 * condition counts: it is an ordinary comparison in the AST, and F Prime
 * treats an assertion as a valid check -- projects choose their own assert
 * level. A bound with no constant value is accepted, since the query cannot
 * show it is too small.
 */
bindingset[k]
predicate hasCheck(Function f, string name, float k) {
  exists(RelationalOperation rop, Expr sizeSide, Expr bound |
    rop.getEnclosingFunction() = f and
    sizeSide = rop.getAnOperand() and
    bound = rop.getAnOperand() and
    bound != sizeSide and
    exists(Expr read | read = sizeSide.getAChild*() and readsSize(read, name)) and
    (
      not exists(bound.getValue())
      or
      bound.getValue().toFloat() >= k - 1.0
    )
  )
}

from SubExpr sub, Call minuend, Expr subtrahend, float k
where
  sub.getUnspecifiedType().(IntegralType).isUnsigned() and
  minuend.getParent*() = sub.getLeftOperand() and
  isSizeAccessor(minuend) and
  subtrahend = sub.getRightOperand() and
  k = subtrahend.getValue().toFloat() and
  not hasCheck(sub.getEnclosingFunction(), minuend.getTarget().getName(), k)
select sub,
  "Unsigned subtraction from " + minuend.getTarget().getName() +
    "() with no check that it is at least the subtracted constant; this underflows instead of going negative when the size is too small."
