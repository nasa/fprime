//! \brief Stub implementation of atomic for single-threaded platforms
//!
//! This is a minimal stub implementation of atomic that provides the
//! same interface as the standard std::atomic but without actual atomic
//! operations. This is suitable for single-threaded applications on
//! platforms that do not natively support std::atomic.
//!
//! NOTE: This file is meant to be included inside a namespace declaration
//!
//! \tparam T The type to be wrapped in the atomic
//!
template<typename T>
class atomic {
private:
    T value;  //!< The stored value

public:
    // Type definitions to match std::atomic interface
    typedef T value_type;

    //! \brief Default constructor - value is uninitialized
    //!
    atomic() = default;

    //! \brief Constructor with initial value
    //! \param desired The initial value
    //!
    constexpr atomic(T desired) : value(desired) {}

    //! \brief Copy constructor (deleted to match std::atomic behavior)
    //!
    atomic(const atomic&) = delete;

    //! \brief Assignment operator (deleted to match std::atomic behavior)
    //!
    atomic& operator=(const atomic&) = delete;

    //! \brief Assignment operator
    //! \param desired The value to assign
    //! \return The assigned value
    //!
    T operator=(T desired) {
        value = desired;
        return desired;
    }

    //! \brief Pre-increment operator
    //! \return Reference to the incremented value
    //!
    T operator++() {
        return ++value;
    }

    //! \brief Post-increment operator
    //! \return The value before increment
    //!
    T operator++(int) {
        return value++;
    }

    //! \brief Pre-decrement operator
    //! \return Reference to the decremented value
    //!
    T operator--() {
        return --value;
    }

    //! \brief Post-decrement operator
    //! \return The value before decrement
    //!
    T operator--(int) {
        return value--;
    }

    //! \brief Load the atomic value
    //! \return The current value
    //!
    T load() const {
        return value;
    }

    //! \brief Store a value atomically
    //! \param desired The value to store
    //!
    void store(T desired) {
        value = desired;
    }

    //! \brief Compare and exchange (weak version)
    //! \param expected Reference to expected value
    //! \param desired The value to store if comparison succeeds
    //! \return true if the exchange was successful
    //!
    bool compare_exchange_weak(T& expected, T desired) {
        if (value == expected) {
            value = desired;
            return true;
        } else {
            expected = value;
            return false;
        }
    }

    //! \brief Compare and exchange (strong version)
    //! \param expected Reference to expected value
    //! \param desired The value to store if comparison succeeds
    //! \return true if the exchange was successful
    //!
    bool compare_exchange_strong(T& expected, T desired) {
        if (value == expected) {
            value = desired;
            return true;
        } else {
            expected = value;
            return false;
        }
    }

    //! \brief Atomic fetch and add
    //! \param arg The value to add
    //! \return The value before the addition
    //!
    T fetch_add(T arg) {
        T old_value = value;
        value += arg;
        return old_value;
    }

    //! \brief Atomic fetch and subtract
    //! \param arg The value to subtract
    //! \return The value before the subtraction
    //!
    T fetch_sub(T arg) {
        T old_value = value;
        value -= arg;
        return old_value;
    }

    //! \brief Conversion operator to T
    //! \return The current value
    //!
    operator T() const {
        return load();
    }
};
