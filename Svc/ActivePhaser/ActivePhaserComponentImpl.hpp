// ======================================================================
// \title  ActiveRateGroupComponentImpl.hpp
// \author mstarch
// \brief  hpp file for ActiveRateGroup component implementation class
//
// \copyright
// Copyright 2009-2015, by the California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
//
// ======================================================================

#ifndef ActiveRateGroup_HPP
#define ActiveRateGroup_HPP

#include "Os/Mutex.hpp"
#include "Common/ActivePhaser/ActivePhaserComponentAc.hpp"

namespace Common {

class ActivePhaserComponentImpl : public ActivePhaserComponentBase {
  public:
    static const U32 MAX_CHILDREN = 100;
    static const U32 DONT_CARE = 0xFFFFFFFFlu;

    enum PhaserContextType {
        SEQUENTIAL,
        COUNT
    };

    /**
     * \brief configuration for phasing
     */
    struct PhaserStateEntry {
        U32 port;
        U32 start;
        U32 length;
        U32 context;
        PhaserContextType contextType;
        bool started;
    };

    struct PhaserStateTable {
        U32 used;
        U32 current;
        PhaserStateEntry entries[MAX_CHILDREN];
    };

    // ----------------------------------------------------------------------
    // Construction, initialization, and destruction
    // ----------------------------------------------------------------------

    //! Construct object ActiveRateGroup
    //!
    ActivePhaserComponentImpl(const char* const compName /*!< The component name*/
    );

    //! Initialize object ActiveRateGroup
    //!
    void init(const NATIVE_INT_TYPE queueDepth,  /*!< The queue depth*/
              const NATIVE_INT_TYPE instance = 0 /*!< The instance number*/
    );

    void configure(U32 cycle_ticks);

    //! Destroy object ActiveRateGroup
    //!
    ~ActivePhaserComponentImpl(void);

    void register_phased(U32 port,  U32 length, U32 start=DONT_CARE, U32 context=DONT_CARE);

  PRIVATE:
    // ----------------------------------------------------------------------
    // Handler implementations for user-defined typed input ports
    // ----------------------------------------------------------------------

    //! Handler implementation for CycleIn
    //!
    void CycleIn_handler(const NATIVE_INT_TYPE portNum, /*!< The port number*/
                         Svc::TimerVal& cycleStart      /*!< Cycle start timer value*/
    );

    //! Handler implementation for Tick internal interface
    //!
    void Tick_internalInterfaceHandler();


    //! Handle a finishing task
    //!
    bool finishChild(U32 current_ticks);

    //! Handle a starting task
    //!
    void startChild(U32 current_ticks);


    //! Auto-incrementing context helper
    //!
    U32 getNextContext(U32 port);

    //! Calculating the time in cycle
    //!
    U32 timeInCycle(U32 full_ticks);

    Os::Mutex m_lock;
    U32 m_cycle;
    U32 m_ticks;
    U32 m_last_start_ticks;
    U32 m_last_cycle_ticks;
    U32 m_cycle_count;
    PhaserStateTable m_state;
    bool m_skip;
};

}  // end namespace Svc

#endif
