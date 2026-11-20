;; A `main` nested 16 blocks deep: needs 17 validator control frames
;; (the function body counts as one). Loads under the tester's standard decoder
;; limits but is rejected once `decoderControlFrames` is configured below that.
(module
  (memory 1 (pagesize 1))
  (export "memory" (memory 0))
  (export "main" (func $main))
  (func $main
    block
    block
    block
    block
    block
    block
    block
    block
    block
    block
    block
    block
    block
    block
    block
    block
    end
    end
    end
    end
    end
    end
    end
    end
    end
    end
    end
    end
    end
    end
    end
    end))
