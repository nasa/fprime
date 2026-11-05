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
 *
 * A check only counts when it reads the size of the *same object* that the
 * subtraction reads. Without that, any comparison of any object's size
 * anywhere in the enclosing function suppresses the finding, which is how
 * the original #5518 defect escaped: `procRequest_handler` is ~330 lines and
 * compares the sizes of three other buffers, never `fwBuffer`.
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
 * Gets the object that the size accessor call `c` reads from, identified by
 * the declaration its receiver resolves to: the variable or field accessed,
 * or the enclosing class when the call is on `this`. Comparing declarations
 * rather than expressions makes `this->m_buffer.getSize()` in a check match
 * `this->m_buffer.getSize()` in a subtraction.
 *
 * Has no result when the receiver is not one of those shapes, for example
 * `getBuffer().getSize()`.
 */
Declaration sizeAccessorReceiver(Call c) {
  isSizeAccessor(c) and
  exists(Expr recv | recv = c.getQualifier() |
    result = recv.(VariableAccess).getTarget()
    or
    recv instanceof ThisExpr and
    result = c.getTarget().(MemberFunction).getDeclaringType()
  )
}

/**
 * Holds if the size accessor calls `a` and `b` read from the same object.
 *
 * When neither receiver resolves to a declaration the calls are treated as
 * matching. That keeps the previous name-only behaviour for shapes this
 * query cannot reason about, rather than reporting them.
 */
predicate sameReceiver(Call a, Call b) {
  sizeAccessorReceiver(a) = sizeAccessorReceiver(b)
  or
  not exists(sizeAccessorReceiver(a)) and
  not exists(sizeAccessorReceiver(b)) and
  isSizeAccessor(a) and
  isSizeAccessor(b)
}

/**
 * Holds if `read` reads the same size, from the same object, as the accessor
 * call `minuend`: either a direct call, or an access to a local variable
 * whose initializer is such a call. The local-copy form is idiomatic --
 * `const FwSizeType n = buf.getSize();` then a check on `n` -- and a check
 * written that way is still a check; `Fw::DpContainer::setBuffer` does
 * exactly this.
 */
predicate readsSameSize(Expr read, Call minuend) {
  exists(Call acc |
    acc = read.(Call)
    or
    exists(LocalScopeVariable v |
      read = v.getAnAccess() and
      acc = v.getInitializer().getExpr()
    )
  |
    isSizeAccessor(acc) and
    acc.getTarget().getName() = minuend.getTarget().getName() and
    sameReceiver(acc, minuend)
  )
}

/**
 * Holds if the function containing `minuend` compares the size that
 * `minuend` reads -- same accessor, same object -- against a bound that is
 * not demonstrably too small for `minuend - k`. An `FW_ASSERT` condition
 * counts: it is an ordinary comparison in the AST, and F Prime treats an
 * assertion as a valid check -- projects choose their own assert level. A
 * bound with no constant value is accepted, since the query cannot show it
 * is too small.
 */
bindingset[k]
predicate hasCheck(Call minuend, float k) {
  exists(RelationalOperation rop, Expr sizeSide, Expr bound |
    rop.getEnclosingFunction() = minuend.getEnclosingFunction() and
    sizeSide = rop.getAnOperand() and
    bound = rop.getAnOperand() and
    bound != sizeSide and
    exists(Expr read | read = sizeSide.getAChild*() and readsSameSize(read, minuend)) and
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
  not hasCheck(minuend, k)
select sub,
  "Unsigned subtraction from " + minuend.getTarget().getName() +
    "() with no check that it is at least the subtracted constant; this underflows instead of going negative when the size is too small."
