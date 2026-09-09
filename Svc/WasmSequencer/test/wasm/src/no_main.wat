;; Valid module that exports a function named `other` but NOT `main`.
;; moduleHasValidMain fails (no `main` export, ERR_NOT_FOUND) -> MAIN_CHECK /
;; MAIN_CHECK_PENDING_START_AND_MAIN else branch -> reportModuleInvalidMain ->
;; InvalidModuleEntrypoint event -> respond_ERROR -> READY.
(module
  (memory 1 (pagesize 1))
  (export "memory" (memory 0))
  (export "other" (func $other))
  (func $other))
