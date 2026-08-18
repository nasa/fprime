/**
 * @name Use of non-constant function pointer
 * @description Non-constant pointers to functions should not be used.
 * @kind problem
 * @id cpp/fprime/jpl-c/non-const-function-pointer
 * @problem.severity recommendation
 * @precision low
 * @tags maintainability
 *       readability
 *       external/jpl
 */

import cpp

/*
 * F Prime refinement of the stock JPL Rule 29 query
 * (cpp/jpl-c/non-const-function-pointer). In C++ template code that has not
 * been instantiated, calls to dependent member functions (e.g.
 * `this->m_impl.clear()` inside a class template) cannot be resolved to a
 * target and are represented as calls through an expression, which the stock
 * query mistakes for function-pointer calls. The same applies to calls
 * through a template non-type parameter of function-pointer type, which is a
 * compile-time constant. Only calls that appear in fully instantiated
 * (non-template) code are real function-pointer calls.
 */

from ExprCall c
where
  not c.getExpr().getType().isConst() and
  not c.isFromUninstantiatedTemplate(_)
select c, "This call does not go through a const function pointer."
