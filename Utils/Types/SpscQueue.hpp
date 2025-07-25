// ======================================================================
// \title  SpscQueue.hpp
// \brief  Lightweight wait-free non-allocating single producer single consumer queue
//
// This algorithm is lock-free, wait-free, thread-safe, and ISR-safe, but
// it relies on two restrictions to achieve these properties:
//
//    1. There may only be one producer thread, which is the thread
//       that may call produce.
//    2. There may only be one consumer thread, which is the thread
//       that may call consume and peek.
//
// For the purposes of this algorithm, an ISR can be considered to be a
// thread. In addition, multiple threads could share the responsibility of
// being a producer or the responsibility of being a consumer if another
// higher-level concurrency mechanism (like a Mutex) is used to ensure that
// there is only a single thread acting as producer or a single thread acting
// as consumer at any time.
//
// Attempting to produce from multiple threads or consume/peek from multiple
// threads without higher-level synchronization can lead to memory corruption.
//
// The isFull() and isEmpty() operations may be used from either the
// producer or consumer thread, but beware that the answer could potentially
// get outdated, depending on which thread calls it. Here are the valid
// uses:
//
//    1. If isEmpty() returns false when called by the consumer, then the
//       next consume or peek operation is guaranteed to succeed.
//    2. If isFull() returns false when called by the producer, then the
//       next produce operation is guaranteed to succeed.
//
// In addition, this algorithm does not dynamically allocate memory, making
// it robust for hard-real-time environments.
//
// ======================================================================

#ifndef UTILS_TYPES_SPSC_QUEUE_HPP
#define UTILS_TYPES_SPSC_QUEUE_HPP

#include <atomic>

namespace Types {

// Note: FwSizeType is probably generally larger than we need,
// but it should still be an efficient size to manipulate,
// and it's guaranteed to be unsigned, which is crucial.
template <class E, FwSizeType CAPACITY>
class SpscQueue {
  public:
    static_assert(CAPACITY * 2 <= std::numeric_limits<FwSizeType>::max(),
                  "This implementation distinguishes full and empty queues by using indices modulo CAPACITY * 2, "
                  "so CAPACITY * 2 must fit in the index type");

    SpscQueue() : m_elements{}, m_nextProduceIdx(0), m_nextConsumeIdx(0) {
        FW_ASSERT(this->m_nextProduceIdx.is_lock_free() && this->m_nextConsumeIdx.is_lock_free());
    }

    bool isFull() const {
        return countElements(this->m_nextProduceIdx.load(), this->m_nextConsumeIdx.load()) == CAPACITY;
    }

    bool isEmpty() const { return countElements(this->m_nextProduceIdx.load(), this->m_nextConsumeIdx.load()) == 0; }

    // May only be called by the single producer thread.
    bool produce(const E& element) {
        FwSizeType nextProduceIdx = this->m_nextProduceIdx.load();
        FwSizeType nextConsumeIdx = this->m_nextConsumeIdx.load();

        if (countElements(nextProduceIdx, nextConsumeIdx) == CAPACITY) {
            return false;
        }

        this->m_elements[nextProduceIdx % CAPACITY] = element;
        this->m_nextProduceIdx.store((nextProduceIdx + 1) % (CAPACITY * 2));
        return true;
    }

    // May only be called by the single consumer thread.
    bool consume(E& elementOut) {
        FwSizeType nextProduceIdx = this->m_nextProduceIdx.load();
        FwSizeType nextConsumeIdx = this->m_nextConsumeIdx.load();

        if (countElements(nextProduceIdx, nextConsumeIdx) == 0) {
            return false;
        }

        elementOut = this->m_elements[nextConsumeIdx % CAPACITY];
        this->m_nextConsumeIdx.store((nextConsumeIdx + 1) % (CAPACITY * 2));
        return true;
    }

    // May only be called by the single consumer thread.
    bool peek(E& elementOut) const {
        FwSizeType nextProduceIdx = this->m_nextProduceIdx.load();
        FwSizeType nextConsumeIdx = this->m_nextConsumeIdx.load();

        if (countElements(nextProduceIdx, nextConsumeIdx) == 0) {
            return false;
        }

        elementOut = this->m_elements[nextConsumeIdx % CAPACITY];
        return true;
    }

    // May only be called by the single consumer thread.
    bool consume() {
        E ignored;
        return consume(ignored);
    }

  private:
    E m_elements[CAPACITY];
    std::atomic<FwSizeType> m_nextProduceIdx;
    std::atomic<FwSizeType> m_nextConsumeIdx;

    static FwSizeType countElements(FwSizeType nextProduceIdx, FwSizeType nextConsumeIdx) {
        FwSizeType count = (nextProduceIdx - nextConsumeIdx + CAPACITY * 2) % (CAPACITY * 2);
        FW_ASSERT(count <= CAPACITY, nextProduceIdx, nextConsumeIdx, count, CAPACITY);
        return count;
    }
};

}  // namespace Types

#endif
