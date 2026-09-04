;; Executes the `unreachable` instruction -> TRAP UNREACHABLE (mapTrapReason).
(module
  (memory 1 (pagesize 1))
  (export "memory" (memory 0))
  (export "main" (func $main))
  (func $main
    unreachable))
