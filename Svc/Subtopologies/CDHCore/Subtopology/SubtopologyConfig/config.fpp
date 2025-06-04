module Config {
    # Base ID for your subtopology. All instantiated components will be offsets of this
    constant CDHCoreSubtopology_BASE_ID = 0xFFFF0000
    
    # include default Queue and Stack sizes here
    module Defaults {
        constant QUEUE_SIZE = 10
        constant STACK_SIZE = 64 * 1024
    }

    module Priorities {
    }
}