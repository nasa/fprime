# F´ On Baremetal and Multi-Core Systems

F´ supports use on baremetal, multi-core, and even multi-device systems. This guide seeks to walk the user through some
of the caveats and delicacies of such systems. It includes

- [Baremetal Systems](#baremetal-systems)
    - [The Joy of Passive Components](#the-joy-of-passive-components)
    - [Choosing an Execution Context](#choosing-an-execution-context)
- [Multi-Core and Multi-Device Systems](#multi-core-and-multi-device-systems)
- [Thread Virtualization](#thread-virtualization)
    - [Defining Custom Tasks](#defining-custom-tasks)
    - [How It Works](#how-it-works)

## Baremetal Systems

A baremetal system is a system that does not run an Operating System to support the F´ software. Thus, the F´ software
must provide for basic services such as filesystems, thread schedulers, etc, to run.  Since F´ was originally run on
systems that provide these services, a number of precautions must be taken when designing F´systems for baremetal
platforms.

### The Joy of Passive Components

First and foremost, baremetal F´ systems should avoid using **Active Components**  at all costs because these components
require quasi-asynchronous execution contexts in which to run. i.e. they need thread such that they can execute in
"parallel" with each other. **Note:** If you **must** use **Active Components** you should thoroughly review the
[thread virtualization](#thread-virtualization) section of this document and associated technology.

If your system can be entirely defined by **Passive Components** then implicitly every port **invocation** would be
synchronous and the execution context would be entirely delegated to every component.  A discussion of the source
of that delegated execution context comes next.

### Choosing an Execution Context

Since the OS is not around to execute F´, the implementer of the F´ project must choose an execution context for F´ to
run on. That is, ensuring that some call invokes all of the **Components** that compose the F´ system. Otherwise, some
components will not run. Typically, this is handled by composing an F´ baremetal system into components that are all
driven by [rate groups](../best/rate-group.md). Designing the system this way ensures that all execution is derived from
one source: the rate group driver and thus reducing the problem to supplying an execution context to the rate group
driver at a set rate.  All calls needed will execute during a sweep through the rate groups and their derived rates.

**Note:** Other options exist (see [Thread Virtualization](#thread-virtualization) below).

Although a full discussion of supplying execution context to the rate group driver is outside the scope of this
documentation, this author wanted to provide a few tips. First, F´ execution should be primarily derived from the main
program loop. i.e. embedded software typically looks like the following and the loop-forever `execute();` action should
trigger the rate group driver at a set interval.

```C
// Run once setup
setup();

// Do this forever
while (true) {
   execute();
}
```

Now all that is required is to determine when this interval has elapsed. This can be done by spinning on a hardware clock
signal, calculating elapsed time by the reading of clock registers, using timing library functions, the `sleep()` call, or
by a timer-driven interrupt service routine (ISR). **Note:** ISRs are complex items and should be studied in detail
before going this route.  Notably, the ISR should not execute the rate group directly, but rather should set a flag or
queue a start message and allow the `while (true) {}` spin in the main loop to detect this signal and start the rate
groups.

## Multi-Core and Multi-Device Systems

We have yet to see any issues running F´ on multi-core systems. Some users have been successful in scheduling high-priority
components to designated cores.  In general, these systems behave just fine. **Note:** Some portions of F´ use `U32`
types so synchronize between threads. In many systems this is a safe atomic operation, however; this is not guaranteed
in all systems. A project should use care to ensure that its system will behave as expected.  These usages are under
review and will be corrected over time.

When running F´ on multi-platform systems, users typically define a deployment for each platform in the system. These
deployments are then linked over the platform's inter-communication architecture.  Should  users want F´ execution
across these deployments to look like a single F´ deployment, users are advised to adopt the
[hub pattern](../best/hub-pattern.md) to invoke F´ port calls across multiple devices.

## Thread Virtualization

**Note:** This is an experimental technology with respect to F´. Care to understand its implementation should be taken
before using it in a production/flight context.

Some systems, even baremetal systems, require the use of **Active Components**.  Many of the `Svc` components are by
design active components. It is impractical to assume that all projects can, at the moment of conception, discard all
use of the framework provided **Active Components**.  Thus F´ was augmented with the ability to virtualize threading, such
that projects could use these components during development as they migrate to a fully passive-component system.

To activate this feature see: [Configuring F´](../user/configuration.md). Continue reading for all the fancy details of
how this system is designed.  TODO: put a link to the thread scheduler API documentation.

### Defining Custom Tasks

When using the thread virtualization technology, care should be taken with custom tasks/threads. This design, as
described below, is dependent on threads that "run once" and externalize the looping part of the thread. Therefore,
custom tasks must wrap functions that obey the following implementation requirements:

1. The function shall not loop
2. The function shall never block execution
3. The function shall perform "one slice" of the thread and then return

Failure to comply with these requirements will cause the thread virtualization technology to fail, and the F´
application to lock up or otherwise behave erratically.

### How It Works

At the core of **Active Components** is a thread, which typically requires an Os to provide a scheduler for it to run,
and through this scheduler, it gets designated an execution context to run in. Thus threads can execute as if they fully
own their execution context and the Os masks this behind the scenes. The purpose of the thread virtualization when
enabled for an F´ project is to unroll these threads such that they can share a single execution context and the
parallel behavior of the threads is "virtualized". The technique is known as protothreading. We'll explore this concept
with relation to F´ below.

Each F´ thread supporting an Active component can be roughly modeled by the code below.  The thread loops until the
system shuts down. For each iteration through the loop it blocks (pauses execution) until a message arrives. It then
dispatches the message and returns to a blocked state waiting for the next message.

```C++
Component1 :: run_thread() {
    while (!shutdown) {
        msg = block_get_message();
        dispatch(msg);
    }
}
```

Here `block_get_messages();` retrieves messages, blocking until one arrives. This loop could have easily been
implemented using a less-efficient model by iterating continuously through the loop and checking if a message has
arrived and dispatching if and only if a message is available. As can be seen below, the wait-by-blocking has been
replaced by the busy wait of constantly iterating through the loop.

```C++
void Component1 :: run_once() {
    if (message_count() > 0) {
        msg = nonblock_get_message();
        dispatch(msg);
    }
    return;
}

Component1 :: run_thread() {
    while (!shutdown) {
        comp1.run_once();
    }
}
```

Here, we extracted the iteration into a `run_once` function for clarity. The blocking wait in the first function is
replaced with a spin on an if-condition until a message is available, then the dispatch happens.

It should be only a slight extrapolation that one could move all the component `run_once` functions into a single loop
and call each in succession. As long as these calls return in a reasonable amount of time, and none of these calls
block internally, then crude parallelism is achieved.

```C++
while (!shutdown) {
    comp1.run_once();
    comp2.run_once();
    comp3.run_once();
    ...
}
```
Here, as seen above, `run_once` does not block and so each component gets a slice of execution time before yielding to
the next. Parallelism has been virtualized and the processor is shareable without writing a full-blown thread scheduler
nor requiring processor instruction set support to switch threading contexts.

Inside F´ a parallel implementation of the active component task was implemented such that it returns rather than blocks
on receiving messages. When `BAREMERAL_SCHEDULER` is enabled in the F´ configuration, this alternate implementation is
used. Under `Os/Baremetal`, an implementation of a sequential scheduler exists.  This scheduler snoops on task
registration and will call all thread executions in a loop driven from the main program loop similar to below.

```C++
setup(); // Setup F´
while (true) {
    scheduler.run_once();
}
```