#ifndef FW_TRAP_HPP
#define FW_TRAP_HPP
#include <Fw/FPrimeBasicTypes.hpp>

namespace Fw {
/**
 * TrapHandler:
 *   A framework class used to handle traps that occur during the execution of the
 * the F' framework. Must be registered with a trap register. The user should
 * inherit from this class and implement the doTrap function. There is no default
 * implementation: doTrap is pure virtual, so a subclass that does not implement it
 * cannot be instantiated.
 */
class TrapHandler {
  public:
    TrapHandler() {};           //!< constructor
    virtual ~TrapHandler() {};  //!< destructor
    /**
     * Handles the incoming trap.
     * \param trap: trap number
     */
    virtual void doTrap(U32 trap) = 0;
};
}  // namespace Fw
#endif
