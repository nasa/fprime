#ifndef FW_TRAP_HPP
#define FW_TRAP_HPP
#include <FpConfig.hpp>

namespace Fw {
    /**
     * TrapHandler:
     *   A framework class used to handle traps that occur during the execution of the
     * the F' framework. Must be registered with a trap register. The user should
     * inherit from this class and ensure that the doTrap function is implemented. The
     * default implementation will be do-nothing.
     */
    class TrapHandler {
        public:
            TrapHandler() {}; //!< constructor
            virtual ~TrapHandler() {}; //!< destructor
            /**
             * Handles the incoming trap.
             * Note: if user does not supply an implementer of this
             *       function, a do-nothing version will be run.
             * \param trap: trap number
             */
            virtual void doTrap(U32 trap) = 0;
    };
}
#endif
