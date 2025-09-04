// ======================================================================
// \title  ActivePhaser.hpp
// \author mstarch
// \brief  cpp file for ActivePhaser component implementation class
//
// \copyright
// Copyright 2009-2015, by the California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
//
// ======================================================================

#ifndef Svc_ActivePhaser_HPP
#define Svc_ActivePhaser_HPP

#include "Svc/ActivePhaser/ActivePhaserComponentAc.hpp"

namespace Svc {

class ActivePhaser final : public ActivePhaserComponentBase {
    friend class ActivePhaserTester;

  public:
    static const U32 MAX_CHILDREN = 100;
    static const U32 DONT_CARE = 0xFFFFFFFFlu;
    static constexpr FwIndexType CONNECTION_COUNT_MAX = NUM_PHASERMEMBEROUT_OUTPUT_PORTS;

    enum PhaserContextType {
        SEQUENTIAL,  // Context stores the number of times a port is called from the beginning of execution.
        COUNT        // Context stores the number of phaser cycles elapsed within a user-specified time window.
    };

    //! Finish status
    enum FinishStatus {
        UNKNOWN,  //!< Improper finish call: child not running, no child, etc.
        ON_TIME,  //!< Child finished on time
        LATE      //!< Child finished late
    };

    /**
     * \brief configuration for phasing
     */
    struct PhaserStateEntry {
        FwIndexType port;
        U32 start;
        U32 length;
        U32 context;
        PhaserContextType contextType;
        bool started;
    };

    struct PhaserStateTable {
        U32 used;     //!< The number of registered tasks (the last registered task is at used - 1)
        U32 current;  //!< The current child task entry index
        PhaserStateEntry entries[MAX_CHILDREN];
    };

    // ----------------------------------------------------------------------
    // Component construction and destruction
    // ----------------------------------------------------------------------

    //! Construct ActivePhaser object
    ActivePhaser(const char* const compName  //!< The component name
    );

    //! Initialize ActivePhaser object
    //!
    void init(const FwSizeType queueDepth,   /*!< The queue depth*/
              const FwIndexType instance = 0 /*!< The instance number*/
    );

    //! Configure ActivePhaser object
    //!
    void configure(U32 cycle_ticks);

    //! Register a phased port call
    //!
    void register_phased(FwIndexType port, U32 length, U32 start = DONT_CARE, U32 context = DONT_CARE);

    //! Destroy ActivePhaser object
    ~ActivePhaser();

  private:
    // ----------------------------------------------------------------------
    // Handler implementations for typed input ports
    // ----------------------------------------------------------------------

    //! Handler implementation for CycleIn
    void CycleIn_handler(FwIndexType portNum,     //!< The port number
                         Os::RawTime& cycleStart  //!< Cycle start timestamp
                         ) override;

    // ----------------------------------------------------------------------
    // Handler implementations for user-defined internal interfaces
    // ----------------------------------------------------------------------

    //! Handler implementation for Tick
    //!
    //! An internal port for sending data of type T
    void Tick_internalInterfaceHandler() override;

    //! Handle a finishing task
    //!
    FinishStatus finishChild(U32 current_ticks);

    //! Handle a starting task
    //!
    void startChild(U32 current_ticks);

    //! Auto-incrementing context helper
    //!
    U32 getNextContext(FwIndexType port);

    //! Calculating the time in cycle
    //!
    U32 timeInCycle(U32 full_ticks);

    Os::Mutex m_lock;
    U32 m_cycle;           // The number of ticks that makes up a phaser cycle
    U32 m_ticks;           // The current tick count
    U32 m_ticks_rollover;  // Roll-over value for ticks
    U32 m_last_start_ticks;
    U32 m_last_cycle_ticks;
    U32 m_cycle_count;
    PhaserStateTable m_state;
};

}  // namespace Svc

#endif
