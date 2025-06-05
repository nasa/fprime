module CDHCoreConfig {
    # Base ID for your subtopology. All instantiated components will be offsets of this
    constant CDHCore_BASE_ID = 0xFFFF0000
    
    # include default Queue and Stack sizes here
    module Defaults {
        constant cmdDisp_QUEUE_SIZE = 10
        constant cmdDisp_STACK_SIZE = 64 * 1024

        constant eventLogger_QUEUE_SIZE = 10
        constant eventLogger_STACK_SIZE = 10

        constant tlmSend_QUEUE_SIZE = 10
        constant tlmSend_STACK_SIZE = 64 * 1024

        constant $health_QUEUE_SIZE = 25
    }

    module Priorities {
        constant cmdDisp_PRIORITY = 101
        constant $health_PRIORITY = 100
        constant eventLogger_PRIORITY = 99
        constant tlmSend_PRIORITY = 98
    }
}