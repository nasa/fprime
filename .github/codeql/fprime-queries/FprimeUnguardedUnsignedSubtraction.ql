/**
 * @name Unguarded unsigned subtraction from a run-time size
 * @description Subtracting a compile-time constant from a size obtained at run time underflows to a very large value, rather than going negative, when the size is smaller than the constant. A comparison that appears only inside an FW_ASSERT does not count as a guard, because assertions may be compiled out.
 * @kind problem
 * @id cpp/fprime/unguarded-unsigned-subtraction
 * @problem.severity warning
 * @tags correctness
 *       security
 *       external/cwe/cwe-191
 */

import cpp
import FprimeAssertions

/*
 * Motivated by the defect fixed in nasa/fprime#5518, where
 * `DpCompressProc::procRequest_handler` computed
 *
 *     fwBuffer.getSize() - Fw::DpContainer::MIN_PACKET_SIZE
 *
 * on a port-supplied buffer. The invariant `getSize() >= MIN_PACKET_SIZE`
 * was enforced only by an `FW_ASSERT` inside `Fw::DpContainer::setBuffer`,
 * in a different translation unit. With assertions compiled out, a short
 * buffer underflowed the subtraction to a value near `SIZE_MAX` and the
 * downstream chunking loop walked off the allocation.
 *
 * The query is deliberately conservative in three ways, to keep it usable
 * as a CI gate rather than a research tool:
 *
 *   1. The subtrahend must be a compile-time constant. Variable-minus-
 *      variable subtraction is far more common and far more often
 *      intentional, so it is out of scope here.
 *
 *   2. The minuend must come from a known size accessor. A bare integer
 *      subtraction says nothing about intent; `getSize() - K` says the
 *      author is reasoning about a buffer.
 *
 *   3. Guard recognition is direction-agnostic and does no dominance
 *      analysis: any relational comparison in the same function between
 *      the accessor and a sufficiently large constant is accepted,
 *      whether it is written `size < K` (early return) or `size >= K`
 *      (positive form). That admits false negatives -- a check on a path
 *      that does not actually reach the subtraction -- in exchange for a
 *      low false-positive rate.
 *
 * On the guard threshold: a comparison against constant `C` is accepted
 * as guarding `accessor - K` when `C >= K - 1`. The `- 1` matters. It
 * makes `if (size > 0)` count as a guard for `size - 1`, which is the
 * idiomatic way to write that check and would otherwise be reported.
 * Comparing `C` and `K` for exact equality (an earlier version of this
 * query) rejects that guard and produces a false positive. A weaker
 * check such as `if (size > 2)` guarding `size - 8` still fails the
 * threshold and is still reported, which is the desired behaviour.
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

/** Holds if `e` appears inside the condition of an `FW_ASSERT`. */
predicate inFwAssert(Expr e) { exists(FwAssert a | e.getParent*() = a.getAsserted()) }

/**
 * Holds if `f` compares the result of a size accessor named `name`
 * against a constant of at least `k - 1`, outside any `FW_ASSERT` --
 * that is, a guard that still runs when assertions are disabled and
 * that is strong enough to make `accessor - k` safe.
 */
bindingset[k]
predicate hasRuntimeGuard(Function f, string name, float k) {
  exists(RelationalOperation rop, Call acc, Expr bound |
    rop.getEnclosingFunction() = f and
    not inFwAssert(rop) and
    acc.getParent*() = rop and
    isSizeAccessor(acc) and
    acc.getTarget().getName() = name and
    bound.getParent*() = rop and
    bound.getValue().toFloat() >= k - 1.0
  )
}

from SubExpr sub, Call minuend, Expr subtrahend, float k
where
  sub.getUnspecifiedType().(IntegralType).isUnsigned() and
  minuend.getParent*() = sub.getLeftOperand() and
  isSizeAccessor(minuend) and
  subtrahend = sub.getRightOperand() and
  k = subtrahend.getValue().toFloat() and
  not hasRuntimeGuard(sub.getEnclosingFunction(), minuend.getTarget().getName(), k)
select sub,
  "Unsigned subtraction from " + minuend.getTarget().getName() +
    "() with no run-time check that it is at least the subtracted constant; this underflows instead of going negative when the size is too small."
