/**
 * Identification of blocking operations (locks, sleeps, blocking I/O,
 * semaphore waits) and interrupt service routines, shared by the ISR and
 * rate-group queries.
 */

import cpp

/** A call to a known blocking operation. */
class BlockingCall extends FunctionCall {
  BlockingCall() {
    this.getTarget().getName() =
      [
        "sleep", "usleep", "nanosleep", "sem_wait", "sem_timedwait", "pthread_mutex_lock",
        "pthread_cond_wait", "pthread_cond_timedwait", "pthread_join", "select", "poll", "epoll_wait",
        "accept", "connect", "recv", "recvfrom", "recvmsg", "send", "sendto", "sendmsg", "flock",
        "fsync", "msgrcv", "mq_receive"
      ]
    or
    exists(MemberFunction mf | mf = this.getTarget() |
      mf.getDeclaringType().getQualifiedName().matches("Os::%") and
      mf.getName() = ["lock", "lockData", "take", "wait", "delay", "receiveBlocking"]
    )
  }

  string blockingDescription() { result = this.getTarget().getName() }
}

/** An interrupt service routine, identified by naming convention or attribute. */
predicate isIsrFunction(Function f) {
  f.getName().toLowerCase().regexpMatch("(^|.*_)isr(_.*|$)|.*interrupt_?handler.*|(^|.*_)irq(_.*|$)")
  or
  f.getAnAttribute().getName() = "interrupt"
}

/** A rate-group (schedIn) handler of an F Prime component. */
predicate isSchedInHandler(Function f) { f.getName().matches("%schedIn%\\_handler") }
