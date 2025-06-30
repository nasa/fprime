module Svc {
    interface Time {
        @ Port to retrieve time
        sync input port timeGetPort: Fw.Time
    }
}
