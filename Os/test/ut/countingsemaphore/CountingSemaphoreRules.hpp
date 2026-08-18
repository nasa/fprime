// ======================================================================
// \title Os/test/ut/countingsemaphore/CountingSemaphoreRules.hpp
// \brief counting semaphore rule definitions
// ======================================================================
// Stripped when compiled, here for IDEs
#include "RulesHeaders.hpp"

// ------------------------------------------------------------------------------------------------------
// Rule:  Wait
//
// ------------------------------------------------------------------------------------------------------
struct Wait : public ConcurrentRule<Os::Test::CountingSemaphore::Tester> {
    // ----------------------------------------------------------------------
    // Construction
    // ----------------------------------------------------------------------

    explicit Wait(AggregatedConcurrentRule<Os::Test::CountingSemaphore::Tester>& runner);

    // ----------------------------------------------------------------------
    // Public member functions
    // ----------------------------------------------------------------------

    bool precondition(const Os::Test::CountingSemaphore::Tester& state) override;

    void action(Os::Test::CountingSemaphore::Tester& state) override;
};

// ------------------------------------------------------------------------------------------------------
// Rule:  WaitTimeout
//
// ------------------------------------------------------------------------------------------------------
struct WaitTimeout : public ConcurrentRule<Os::Test::CountingSemaphore::Tester> {
    // ----------------------------------------------------------------------
    // Construction
    // ----------------------------------------------------------------------

    explicit WaitTimeout(AggregatedConcurrentRule<Os::Test::CountingSemaphore::Tester>& runner);

    // ----------------------------------------------------------------------
    // Public member functions
    // ----------------------------------------------------------------------

    bool precondition(const Os::Test::CountingSemaphore::Tester& state) override;

    void action(Os::Test::CountingSemaphore::Tester& state) override;
};

// ------------------------------------------------------------------------------------------------------
// Rule:  Post
//
// ------------------------------------------------------------------------------------------------------
struct Post : public ConcurrentRule<Os::Test::CountingSemaphore::Tester> {
    // ----------------------------------------------------------------------
    // Construction
    // ----------------------------------------------------------------------

    explicit Post(AggregatedConcurrentRule<Os::Test::CountingSemaphore::Tester>& runner);

    // ----------------------------------------------------------------------
    // Public member functions
    // ----------------------------------------------------------------------

    bool precondition(const Os::Test::CountingSemaphore::Tester& state) override;

    void action(Os::Test::CountingSemaphore::Tester& state) override;
};
