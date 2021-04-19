#ifndef _WatchdogTimer_hpp_
#define _WatchdogTimer_hpp_

#include <Fw/Types/BasicTypes.hpp>

namespace Os {
    class WatchdogTimer {
        public:
            
            typedef enum {
                WATCHDOG_OK, //!< Call was successful
                WATCHDOG_INIT_ERROR, //!< Timer initialization failed
                WATCHDOG_START_ERROR, //!< Timer startup failed
                WATCHDOG_CANCEL_ERROR //!< Timer cancellation failed
            } WatchdogStatus;
            
            typedef void (*WatchdogCb)(void *);
        
            WatchdogTimer();
            virtual ~WatchdogTimer();
            WatchdogStatus startTicks( I32 delayInTicks, //!< number of ticks to delay. OS/timing dependent
                                        WatchdogCb p_callback,  //!< routine to call on time-out
                                        void* parameter);  //!< parameter with which to call routine);
            
            WatchdogStatus startMs( I32 delayInMs, //!< number of ms to delay.
                                        WatchdogCb p_callback,  //!< routine to call on time-out
                                        void* parameter);  //!< parameter with which to call routine);

            WatchdogStatus restart(void);  //!< restart timer with previous value
            
            
            WatchdogStatus cancel(void); //!< cancel timer
            
            
        private:
            
            I32 m_handle; //!< handle for implementation specific watchdog
            WatchdogCb m_cb; //!< function callback pointer
            void* m_parameter; //!< parameter for timer call
            I32 m_timerTicks; //!< number of ticks for timer.
            I32 m_timerMs; //!< number of milliseconds for timer.
            WatchdogTimer(WatchdogTimer&); //!< disable copy constructor

    };
}

#endif
