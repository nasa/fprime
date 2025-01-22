// ======================================================================
// \title Os/test/ut/queue/RulesHeaders.cpp
// \brief definitions for concurrent running rules
// ======================================================================
#include <STest/Rule/Rule.hpp>
#include <gtest/gtest.h>
#include "Os/Task.hpp"
#include "Os/Mutex.hpp"
#include "Os/Condition.hpp"
#include <list>
#include <map>
#include <iterator>
#ifndef OS_TEST_CONCURRENT_RULE
#define OS_TEST_CONCURRENT_RULE


// Forward declaration of the aggregated concurrent rule
template<typename State> class AggregatedConcurrentRule;

template<typename State> class ConcurrentRule : public STest::Rule<State> {
    friend class AggregatedConcurrentRule<State>;
  public:
    ConcurrentRule(const char *const name, AggregatedConcurrentRule<State>& runner) : STest::Rule<State>(name), m_runner(runner) {
        this->m_runner.add(*this);
        this->m_condition_value = false;
    }

    virtual ~ConcurrentRule() {
        this->m_runner.remove(*this);
    }

    //! Launch this rule asynchronously
    void action_async(State& state) {
        this->is_asynchronous = true;
        this->m_state = &state;
        Os::Task::Arguments arguments(Os::TaskString(this->getName()), this->run, this);
        m_task.start(arguments);
    }

    //! Static function to "apply" this rule using a task
    static void run(void* rule_pointer) {
        ASSERT_NE(rule_pointer, nullptr) << "Passed null input to task";
        ConcurrentRule<State>* self = reinterpret_cast<ConcurrentRule<State>*>(rule_pointer);
        ASSERT_TRUE(self->is_asynchronous) << "Rule " << self->getName() << " not run in aggregated concurrent rule";
        ASSERT_TRUE(self->precondition(*self->m_state)) << "precondition failed applying rule " << self->getName();

        // Ensure this rule owns the global state
        Os::ScopeLock lock(self->getLock());
        self->action(*self->m_state);
    }

  protected:
    //! \brief wait until notified to take the next step
    void wait_for_next_step() {
        ASSERT_TRUE(this->is_asynchronous) << "Rule " << this->getName() << " not run in aggregated concurrent rule";
        while (not this->m_condition_value) {
            this->m_condition.wait(this->getLock());
        }
        this->m_condition_value = false;
    }

    //! \brief get lock
    Os::Mutex& getLock() {
        return this->m_runner.getLock();
    }

    //! \brief notify another rule by nae,
    void notify_other(std::string other) {
        this->m_runner.notify(other);
    }

    //! \brief get the condition variable
    bool getCondition() {
        return this->m_condition_value;
    }

  protected:
    void join() {
        this->m_task.join();
    }

    //! \brief notify this rule to take the next step
    void step() {
        this->m_condition_value = true;
        this->m_condition.notify();
    }

    //! Reference to the runner
    AggregatedConcurrentRule<State>& m_runner;

    //! Local store of state for concurrent action
    State* m_state;
    //! Task to run this rule in asynchronous context
    Os::Task m_task;
    //! Condition variable used to set up this rule
    Os::ConditionVariable m_condition;

    //! Condition guarded by condition variable
    std::atomic<bool> m_condition_value;

    //! A check to ensure this rule was launched asynchronously when using the blocking wait function
    bool is_asynchronous = false;
};

template<typename State> class PseudoRule : public ConcurrentRule<State> {
  public:
    PseudoRule(const char* name, AggregatedConcurrentRule<State>& runner)
        : ConcurrentRule<State>(name, runner) {}

    bool precondition(const State& state) override {
        return true;
    }

    void action(State& state) override {}

    void wait_for_next_step() {
        this->ConcurrentRule<State>::wait_for_next_step();
    }


};

template<typename State> class ConcurrentWrapperRule : public ConcurrentRule<State> {
  public:
    ConcurrentWrapperRule(AggregatedConcurrentRule<State>& runner, STest::Rule<State>& wrapped, std::string notify, const char* name=nullptr)
        : ConcurrentRule<State>((name == nullptr) ? wrapped.getName() : name, runner), m_wrapped(wrapped), m_notify(notify) {

    }

    bool precondition(const State& state) override {
        return m_wrapped.precondition(state);
    }

    void action(State& state) override {
        this->wait_for_next_step(); // Wait until told to go
        this->m_wrapped.apply(state); // Go
        this->notify_other(this->m_notify); // Notify when done
    }
  private:

    STest::Rule<State>& m_wrapped;
    std::string m_notify;
};


template<typename State> class AggregatedConcurrentRule : public STest::Rule<State> {
  public:
    //! Creation
    AggregatedConcurrentRule() : STest::Rule<State>("aggregated-rule") {}
    ~AggregatedConcurrentRule() = default;

    //! Add a rule (on construction of rule)
    void add(ConcurrentRule<State>& rule) {
        Os::ScopeLock lock(this->m_lock);
        m_rules.push_back(&rule);
        m_rule_map[rule.getName()] = &rule;
    }

    //! Remove a rule (on destruction of rule)
    void remove(ConcurrentRule<State>& rule) {
        Os::ScopeLock lock(this->m_lock);
        m_rules.remove(&rule);
        // Remove rule from m_rules_map
        for (auto it = m_rule_map.begin(); it != m_rule_map.end(); it++) {
            if (it->second == &rule) {
                m_rule_map.erase(it);
                break;
            }
        }
    }

    // Notify a rule by name
    void notify(std::string& name) {
        // Notify all matching rules
        for (auto pair : m_rule_map) {
            if (std::string(pair.second->getName()) == name) {
                pair.second->step();
                return;
            }
        }
        ASSERT_TRUE(false) << "Failed to find rule to notify";
    }

    Os::Mutex& getLock() {
        return this->m_lock;
    }

    //! Aggregate rule may only run if
    bool precondition(const State& state) override {
        for (ConcurrentRule<State>* rule : m_rules) {
            if (not rule->precondition(state)) {
                return false;
            }
        }
        return true;
    };

    //! Action is to run all rules then wait for all rules to finish
    void action(State& state) override {
        // Launch asynchronous rules
        {
            Os::ScopeLock lock(this->m_lock);
            for (ConcurrentRule<State>* rule : m_rules) {
                rule->action_async(state);
            }
        }
    }

    void join() {
        // Wait for asynchronous rules to finish
        for (ConcurrentRule<State>* rule : m_rules) {
            rule->join();
        }
    }

  private:
    Os::Mutex m_lock;
    std::list<ConcurrentRule<State>*> m_rules;
    std::map<std::string, ConcurrentRule<State>*> m_rule_map;
};

#endif //OS_TEST_CONCURRENT_RULE
