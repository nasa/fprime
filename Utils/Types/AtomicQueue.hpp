// ======================================================================
// \title  AtomicQueue.hpp
// \brief  Lightweight lock-free non-allocating single reader single writer queue
//
// This algorithm is lock-free, wait-free, thread-safe, and ISR-safe, but
// it relies on two restrictions to achieve these properties:
//
//    1. There may only be one writer/producer thread, which is the thread
//       that may call enqueue.
//    2. There may only be one reader/consumer thread, which is the thread
//       that may call dequeue and peek.
//
// For the purposes of this algorithm, an ISR can be considered to be a
// thread. In addition, multiple threads could share the responsibility of
// being a reader or the responsibility of being a writer if another
// higher-level concurrency mechanism (like a Mutex) is used to ensure that
// there is only a single thread acting as reader or a single thread acting
// as writer at any time.
//
// Attempting to enqueue from multiple threads or dequeue/peek from multiple
// threads can lead to memory corruption.
//
// The isFull() and isEmpty() operations may be used from any thread,
// but beware that the answer could potentially get outdated, except when
// used as follows:
//
//    1. If the reader/consumer thread calls isEmpty() and finds that the
//       answer is false, then the next dequeue or peek is guaranteed to
//       succeed.
//    2. If the writer/producer thread calls isFull() and finds that the
//       answer is false, then the next enqueue is guaranteed to succeed.
//
// ======================================================================

#ifndef UTILS_TYPES_ATOMIC_QUEUE_HPP
#define UTILS_TYPES_ATOMIC_QUEUE_HPP

#include <atomic>

namespace Types {

// Note: FwSizeType is probably generally larger than we need,
// but it should still be an efficient size to manipulate,
// and it's guaranteed to be unsigned, which is crucial.
template<class E, FwSizeType SIZE>
class AtomicQueue {
public:
    static_assert(SIZE * 2 <= std::numeric_limits<FwSizeType>::max(),
        "This implementation distinguishes full and empty queues by using indices modulo SIZE * 2,"
        "so SIZE * 2 must fit in the index type");

    AtomicQueue() : m_elements{}, m_nextEnqueueIdx(0), m_nextDequeueIdx(0) {}

    bool isFull() const {
        return countElements(this->m_nextEnqueueIdx.load(), this->m_nextDequeueIdx.load()) == SIZE;
    }

    bool isEmpty() const {
        return countElements(this->m_nextEnqueueIdx.load(), this->m_nextDequeueIdx.load()) == 0;
    }

    // Single writer
    bool enqueue(const E &element) {
        FwSizeType nextDequeueIdx = this->m_nextDequeueIdx.load();
        FwSizeType nextEnqueueIdx = this->m_nextEnqueueIdx.load();

        if (countElements(nextEnqueueIdx, nextDequeueIdx) == SIZE) {
            return false;
        }

        this->m_elements[nextEnqueueIdx % SIZE] = element;
        this->m_nextEnqueueIdx.store((nextEnqueueIdx + 1) % (SIZE * 2));
        return true;
    }

    // Single reader
    bool dequeue(E &elementOut) {
        FwSizeType nextDequeueIdx = this->m_nextDequeueIdx.load();
        FwSizeType nextEnqueueIdx = this->m_nextEnqueueIdx.load();

        if (countElements(nextEnqueueIdx, nextDequeueIdx) == 0) {
            return false;
        }

        elementOut = this->m_elements[nextDequeueIdx % SIZE];
        this->m_nextDequeueIdx.store((nextDequeueIdx + 1) % (SIZE * 2));
        return true;
    }

    bool peek(E &elementOut) const {
        FwSizeType nextDequeueIdx = this->m_nextDequeueIdx.load();
        FwSizeType nextEnqueueIdx = this->m_nextEnqueueIdx.load();

        if (countElements(nextEnqueueIdx, nextDequeueIdx) == 0) {
            return false;
        }

        elementOut = this->m_elements[nextDequeueIdx % SIZE];
        return true;
    }

    bool dequeue() {
        E ignored;
        return dequeue(ignored);
    }

private:
    E m_elements[SIZE];
    std::atomic<FwSizeType> m_nextEnqueueIdx;
    std::atomic<FwSizeType> m_nextDequeueIdx;

    static FwSizeType countElements(FwSizeType nextEnqueueIdx, FwSizeType nextDequeueIdx) {
        FwSizeType count = (nextEnqueueIdx - nextDequeueIdx + SIZE * 2) % (SIZE * 2);
        FW_ASSERT(count <= SIZE, nextEnqueueIdx, nextDequeueIdx, count, SIZE);
        return count;
    }
};

}  // namespace Types

#endif
