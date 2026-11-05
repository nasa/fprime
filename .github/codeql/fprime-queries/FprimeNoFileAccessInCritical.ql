/**
 * @name Synchronous file access in critical region or rate-group handler
 * @description Synchronous file I/O has unbounded latency and must not be
 *              performed while holding a lock or inside a rate-group
 *              (schedIn) handler.
 * @kind problem
 * @id cpp/fprime/no-file-access-in-critical
 * @problem.severity warning
 * @tags correctness
 *       reliability
 *       external/jpl
 */

import cpp
import FprimeBlocking

/** A call performing synchronous file access. */
class FileAccessCall extends FunctionCall {
  FileAccessCall() {
    this.getTarget().getName() =
      [
        "open", "openat", "close", "read", "write", "pread", "pwrite", "lseek", "fopen", "fclose",
        "fread", "fwrite", "fgets", "fputs", "fprintf", "fscanf", "fseek", "fflush", "stat", "fstat",
        "unlink", "rename", "mkdir", "rmdir", "sync", "fsync"
      ] and
    not this.getTarget() instanceof MemberFunction
    or
    exists(MemberFunction mf | mf = this.getTarget() |
      mf.getDeclaringType().getQualifiedName() = ["Os::File", "Os::Directory", "Os::FileSystem"] and
      mf.getName() = ["open", "close", "read", "write", "seek", "flush", "crc32", "readDirectory"]
    )
  }
}

/** A call that acquires a lock. */
predicate lockCall(FunctionCall fc) {
  fc.getTarget().getName() = ["pthread_mutex_lock", "sem_wait"] and
  not fc.getTarget() instanceof MemberFunction
  or
  fc.getTarget().(MemberFunction).getName() = ["lock", "lockData", "take"]
}

/** A call that releases a lock. */
predicate unlockCall(FunctionCall fc) {
  fc.getTarget().getName() = ["pthread_mutex_unlock", "sem_post"] and
  not fc.getTarget() instanceof MemberFunction
  or
  fc.getTarget().(MemberFunction).getName() = ["unLock", "unlock", "unlockData", "release"]
}

/** CFG nodes reachable from `lock` without passing an unlock call. */
predicate lockedFlow(FunctionCall lock, ControlFlowNode n) {
  lockCall(lock) and n = lock.getASuccessor()
  or
  exists(ControlFlowNode mid |
    lockedFlow(lock, mid) and
    not unlockCall(mid) and
    n = mid.getASuccessor()
  )
}

from FileAccessCall fa, string context
where
  fa.fromSource() and
  (
    isSchedInHandler(fa.getEnclosingFunction()) and
    context = "rate-group handler " + fa.getEnclosingFunction().getName()
    or
    lockedFlow(_, fa) and
    not isSchedInHandler(fa.getEnclosingFunction()) and
    context = "a critical region (lock held)"
  )
select fa,
  "Synchronous file access " + fa.getTarget().getName() + " performed in " + context + "."
