#ifndef _LOCKLESS_QUEUE_H_
#define _LOCKLESS_QUEUE_H_

#include <FpConfig.hpp>
#include <Os/Queue.hpp>
#ifndef BUILD_DARWIN // Allow compiling
#include <mqueue.h>
#endif

namespace Os {

  class LocklessQueue {

    typedef struct QueueNode_s {
      U8                 * data;
      NATIVE_INT_TYPE      size;
      struct QueueNode_s * next;
    } QueueNode;
    
  public:
    
    LocklessQueue (NATIVE_INT_TYPE maxmsg, NATIVE_INT_TYPE msgsize);
    ~LocklessQueue();

#ifndef BUILD_DARWIN // Allow compiling
    void GetAttr (mq_attr & attr);
#endif

    Os::Queue::QueueStatus Send (const U8 * buffer, NATIVE_INT_TYPE size);
    Os::Queue::QueueStatus Receive (U8 * buffer, NATIVE_INT_TYPE capacity, NATIVE_INT_TYPE & size);

  private:
    bool PopFree (QueueNode ** free_node);
    void PushFree (QueueNode * my_node);

    QueueNode *  m_first;   
    QueueNode *  m_last;    

    QueueNode *  m_free_head;

    QueueNode *  m_index;
    U8        *  m_data;
#ifndef BUILD_DARWIN
    mq_attr      m_attr;
#endif
  };

}

#endif
