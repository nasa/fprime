/**
 * @name Numeric constant defined with #define
 * @description Numeric constants should be typed constants (`static const`,
 *              `constexpr`, or `enum`) or FPP constants, not preprocessor
 *              macros, so that they carry a type and obey scope. Configuration
 *              headers are excluded.
 * @kind problem
 * @id cpp/fprime/define-constant
 * @problem.severity recommendation
 * @tags maintainability
 *       external/jpl
 */

import cpp

from Macro m
where
  m.fromSource() and
  not m.getFile().getAbsolutePath().matches(["%/config/%", "%/test/%", "%/ut/%", "%Ac.hpp", "%Ac.cpp"]) and
  m.getBody()
      .regexpMatch("\\(?\\s*[-+]?(0[xX][0-9a-fA-F]+|[0-9]+(\\.[0-9]*)?([eE][-+]?[0-9]+)?)[uUlLfF]*\\s*\\)?")
select m, "Numeric constant " + m.getName() + " is defined with #define; use a typed constant."
