# How-To: Test-Driven Development in F Prime (F´)

This guide shows a practical, repeatable test-driven development loop for building an F´ component. You’ll model behavior first in FPP, stub the implementation, write tests that fail, then implement the component until the tests pass—iterating as needed.

---

## Prerequisites

Before starting, you should have:

* Completed the [Hello World](https://fprime.jpl.nasa.gov/latest/tutorials-hello-world/docs/hello-world/) tutorial (so you’ve built at least one component and ran `fprime-util`).
Completed the [LED Blinker](https://fprime.jpl.nasa.gov/latest/tutorials-led-blinker/docs/led-blinker/) tutorial for F´ (so you’ve seen F´ unit-testing).
* A **general understanding of test-driven development** (write a failing test first, make it pass, then refactor).
* A working knowledge of [FPP component modeling](https://nasa.github.io/fpp/fpp-users-guide.html) (ports, commands, events, telemetry, parameters).

> [!TIP]
> Make sure your unit-test development environment is set-up via `fprime-util generate`.

---

## Example Component

To keep things concrete, we’ll walk through a tiny example: a `Counter` component that:

* Accepts an input port call `increment`.
* Emits telemetry `Count` with the current count.

You can swap this for your real component; the test-driven development process stays the same.

> [!TIP]
> You can create a new component with `fprime-util new --component`

---

## The Test-Driven Development Process for F´

In short, the test-driven development process in F´ is: design, test, and implement to the test. This places testing before component implementation as opposed to the traditional process: design, implement, and test the implementation.

### Step 1 — Design the Component

Start by expressing the component's interface as an FPP model. Our example component looks like the following:

```python
module Demo {

  @ A simple counting component. Increments by input and wraps to zero at Max.
  passive component Counter {
    @ Count of the incoming port calls
    telemetry Count: FwSizeType

    @ A no-argument port triggering implementation
    guarded input port increment: Fw.Signal
  }

}
```

**Why design in FPP first?**
In F´ the FPP model is the source of truth that drives code generation. Modeling first ensures you can take advantage of the generated test harness and autocoded functions when writing your tests.

---

### Step 2 — Generate Blank Implementation Classes


Ensure your component exists under a module (e.g., `Demo/Counter/`) and is included in your `CMakeLists.txt`.  Then generate the implementation files for your component using: `fprime-util impl`. If this is your first iteration of the test-driven process, then copy or rename the template into their correct place (i.e. `Counter.cpp` and `Counter.hpp`).

If you are iterating on the design, copy over the new blank handlers, and leave them blank.

**Why?**

Generating (blank) implementation classes will allow the auto code to compile, but implementation has not begun. This will allow us to compile and run tests without compilation errors.

---

### Step 3 — Write Tests to Ensure Correct Behavior

Now we will implement tests that ensure our implementation is working correctly before we've written the implementation!

Next generate the implementation files for the unit-tests. If this is a second iteration, you'll need to copy over updated code.

> [!TIP]
> Remember to add or uncomment a call to `register_fprime_ut` in the `CMakeLists.txt` of your component!

```
fprime-util generate --ut
fprime-util impl --ut
```



Move the files into place and implement test(s) as you see fit.  Below is a test that will ensure our counter component responds correctly to calls to the `increment` port.

```c++
void Tester::test_increment() {
    // Loop a random number of times, ensuring that telemetry matches the current count
    for (U32 i = 0; i < STest::Pick::lowerUpper(1, 10000)) {
        this->invoke_to_increment(0);
        ASSERT_TLM_Count(i, i + 1);
    }
}
```

Add your test(s) and ensure they fail when running `fprime-util check`.

**Why do the tests fail?**

The art of test-driven development is to focus on writing correct tests that ensure correct behavior, then implementing the code to pass the tests. Since implementation has not happened, our test will fail.

---

### Step 4 — Implement the Component

Now that we have a test to check the behavior of this component, we can implement the component iterating until the tests pass!  The result: a good component and matching tests.

The below handler should cause the test to pass.

```c++
void Counter::increment_handler(FwIndexType portNum) {
    this->m_count++;
    this->tlmWrite_Count(m_count);
  }
```

> [!TIP]
> Remember to define `m_count` and initialize it in the `.hpp`.

Re-run the tests:

```bash
fprime-util check
```

They should now pass. If not, adjust the implementation or the tests until the intended behavior is captured and verified.

---

### Step 5 — Rinse and Repeat

You can now repeat the process by tuning the design (FPP), adding more tests, and implementing pass all the tests!


---

## Tips, and Good Practices

* **Model to Drive Tests**: Treat the FPP model as your interface; tests assert that the implementation correctly supports the interface.
* **One Behavior per Test**: Small, focused tests are easier to debug and help pinpoint where the implementation needs improvement.
* **Keep Stubs Compiling**: Even “blank” implementations should compile; let the **assertions** be the source of failure, not build errors.

---

## Recap (Test-Driven Development in F´)

1. **Model in FPP**: design first
2. **Write Tests**: write tests that verify the design and interface
3. **Implement to Pass Tests**: when the implementation passes the test, it is verified (and comes with tests as well)!


## Project Structure (Reference)

For reference, here is the layout of our demo component.

```
Demo/
  Counter/
    CMakeLists.txt
    Counter.fpp
    Counter.hpp
    Counter.cpp
    test/
      ut/
        CounterTester.hpp
        CounterTester.cpp
        CounterTestMain.cpp
```
