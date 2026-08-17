// ======================================================================
// \title config/OsDelegateConditionVariable.hpp
// \brief configured selection of Os OSAL implementations
//
// This header configures how Os::ConditionVariable resolves to a concrete implementation.
// Two mechanisms are available:
//
// 1. Link-time selection (default): Os::ConditionVariable aliases to Os::DelegateConditionVariable,
//    which wraps a ConditionVariableInterface reference. At construction, DelegateConditionVariable
//    calls ConditionVariableInterface::getDelegate() to construct the platform-specific
//    implementation (e.g., PosixConditionVariable, StubConditionVariable) via placement-new.
//    The linker selects which getDelegate() based on which DefaultConditionVariable.cpp
//    is linked. Calls dispatch through the vtable at runtime.
//
// 2. Compile-time selection (performance optimization): Platforms may override
//    this header to alias Os::ConditionVariable directly to a concrete implementation.
//    This eliminates wrapper and virtual dispatch, enabling inlining and aggressive
//    LTO optimization.
//
// Example compile-time selection override:
//
//     namespace SomePlatform { class ConcreteConditionVariable; }
//     namespace Os { using ConditionVariable = SomePlatform::ConcreteConditionVariable; }
//     #define OS_CONDITION_VARIABLE_HEADER "SomePlatform/Os/ConcreteConditionVariable/ConcreteConditionVariable.hpp"
//
// IMPORTANT: CIRCULAR DEPENDENCY PREVENTION
//
//   - This header MUST NOT include any Os OSAL headers (Os/*.hpp).
//   - Only forward-declare types and define the Os::ConditionVariable alias.
//   - OS_CONDITION_VARIABLE_HEADER should point to the implementation header, which will
//     be included AFTER ConditionVariableInterface.hpp in Os/ConditionVariable.hpp.
//   - Violating this constraint will create circular dependencies.
// ======================================================================
#ifndef CONFIG_OS_DELEGATECONDITIONVARIABLE_HPP
#define CONFIG_OS_DELEGATECONDITIONVARIABLE_HPP

//!< Forward declaration of the link-time delegate
//!< Default: select the ConditionVariable implementation at link time
namespace Os {

class DelegateConditionVariable;
using ConditionVariable = DelegateConditionVariable;

}  // namespace Os

#define OS_CONDITION_VARIABLE_HEADER <Os/DelegateConditionVariable.hpp>

#endif  // CONFIG_OS_DELEGATECONDITIONVARIABLE_HPP
