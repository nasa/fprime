@ CFDP ID to denote the current node when sending PDUs
param LocalEid: Cfdp.EntityId \
    default 42

@ Maximum number of bytes to put into a file PDU
param OutgoingFileChunkSize: U32 \
    default 480

@ The maximum number of received bytes to calculate a CRC for in a single wakeup period
param RxCrcCalcBytesPerWakeup: U32 \
    default 16384

@ Default CFDP channel for fileIn port-initiated file transfers
param FileInDefaultChannel: U8 \
    default 0

@ Default destination entity ID for fileIn port-initiated file transfers
param FileInDefaultDestEntityId: Cfdp.EntityId \
    default 100

@ Default CFDP class for fileIn port-initiated file transfers
param FileInDefaultClass: Cfdp.Class \
    default Cfdp.Class.CLASS_2

@ Default file retention policy for fileIn port-initiated file transfers
param FileInDefaultKeep: Cfdp.Keep \
    default Cfdp.Keep.DELETE

@ Default priority for fileIn port-initiated file transfers
param FileInDefaultPriority: U8 \
    default 0

@ Parameter configuration for an array CFDP channels
param ChannelConfig: Cfdp.ChannelArrayParams \
    default [ \
        {
            ack_limit = 4, \
            nack_limit = 4, \
            ack_timer = 3, \
            inactivity_timer = 30, \
            dequeue_enabled = Fw.Enabled.ENABLED, \
            move_dir = "", \
            max_outgoing_pdus_per_cycle = 32, \
            tmp_dir = "/tmp", \
            fail_dir = "/fail" \
        }, \
        {
            ack_limit = 4, \
            nack_limit = 4, \
            ack_timer = 3, \
            inactivity_timer = 30, \
            dequeue_enabled = Fw.Enabled.ENABLED, \
            move_dir = "", \
            max_outgoing_pdus_per_cycle = 32, \
            tmp_dir = "/tmp", \
            fail_dir = "/fail" \
        } \
    ]