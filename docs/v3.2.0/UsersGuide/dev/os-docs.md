# Operating System Abstraction Layer (OSAL)

The framework incorporates an OSAL providing service abstraction classes of a fictitious operating system that can perform all the operations of a real operating system.
This layer shows the user an abstraction of the common functionality provided by all real-time operating systems (RTEMS, VxWorks, Azure ThreadX, QNX, FreeRTOS, Zephyr,...) or not (Linux, MacOS, WinCE,...). We find the following services:

1. multitasking management with prioritization ;
2. synchronization ;
3. file management;
4. message queues;
5. communication; and
6. timeout.

OSAL provides each of these services by wrapping the concrete operating system entities it encapsulates in thin wrappers.
This layer enables the development and testing of adaptable software systems that do not depend on any particular operating system, allowing development to be performed at the development workstation. It reduces the time and effort required for developers to migrate a software system from one operating system to another.

Implementations of these classes are provided for Unix (POSIX) and Mac OS variants. Additional ports can be realized by
providing additional implementations when a new operating system is added. This guide will walk through the critical files in this layer
to allow developers to understand and provide an overview for those looking to port to a new operating system.

This guide discusses the following:
- [Tasks](#tasks)
- [Task Registry](#task-registry)
- [Mutexes](#mutexes)
- [Message Queues](#message-queues)
- [Interval Timer](#interval-timer)
- [Watchdog Timer](#watchdog-timer)
- [Interrupt Lock](#interrupt-lock)
- [File](#file)
- [FileSystem](#file-system)
- [Log](#log)

## Tasks

Tasks are called threads under Unix variants, and tasks under other OSes. A
developer may wish to start tasks to wait on an event or a resource that
is not a port invocation. The Os task class definition can be found in
**Os/OsTask.hpp**. The OS implementations are found in various
subdirectories. Active components require the use of tasks to implement their
features. Table 23 describes the methods of the class.

**Table 23.** Task method descriptions.

| Method                 | Description                                                                                                                                                                       |                                                                                                                                                |
| ---------------------- | --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- | ---------------------------------------------------------------------------------------------------------------------------------------------- |
| start()                | Starts the task. The arguments are:                                                                                                                                               |                                                                                                                                                |
|                        | **Argument**                                                                                                                                                                      | **Description**                                                                                                                                |
|                        | name                                                                                                                                                                              | String name of the task.                                                                                                                       |
|                        | identifier                                                                                                                                                                        | A user-selected unique integer identifying the task.                                                                                           |
|                        | priority                                                                                                                                                                          | The priority of the task: 0 is lowest, 255 is highest.                                                                                         |
|                        | stackSize                                                                                                                                                                         | The size of the stack, in bytes.                                                                                                               |
|                        | routine                                                                                                                                                                           | The function that will be called in the new task context.                                                                                      |
|                        | arg                                                                                                                                                                               | An argument passed to the thread. The arg argument to the routine will be set to this value.                                                   |
|                        | cpuAffinity                                                                                                                                                                       | In SMP systems, specify a processor core to run the task. A value of -1 means no preference.                                                       |
| getIdentifier()        | Returns the task identifier passed in the start() function. Useful when you have a collection of tasks to iterate over.                                                           |                                                                                                                                                |
| getOsIdentifier()      | Gets the Os Task ID. Useful for passive components.                                                                                                                               |                                                                                                                                                |
| getRawHandle()         | Returns the task-handle owned by this task                                                                                                                                        |                                                                                                                                                |
| delay()                | Suspends execution of the task for the specified number of milliseconds.                                                                                                          |                                                                                                                                                |
| getNumTasks()          | Returns the number of active tasks in the system.                                                                                                                                 |                                                                                                                                                |
| suspend()              | Suspends the execution of the task. Not available on all operating systems.                                                                                                       |                                                                                                                                                |
| resume()               | Resumes execution of the task after a suspend() was issued. Not available on all operating systems.                                                                               |                                                                                                                                                |
| wasSuspended()         | Returns a Boolean to indicate whether or not the task was suspended via the suspend() call, or if it was suspended due to some other issue such as an exception.                 |                                                                                                                                                |
| isSuspended()          | Check to see if the task is currently suspended.                                                                                                                                  |                                                                                                                                                |
| setStarted()           | Tells the task object that the task routine was called. Should be called from the routine registered in the start() call.                                                         |                                                                                                                                                |
| isStarted()            | Returns true if the task routine was started successfully. Set by setStarted().                                                                                                   |                                                                                                                                                |
| registerTaskRegistry() | Allows a task registry to be passed for registering the task. This is a static call that should be called only once to register a singleton.                                      |                                                                                                                                                |

## Task Registry

Task registries are meant to be used to track all task instances in the
system. The concept is that this registry would contain pointers to all
the instances and be able to iterate over them and perform actions. The
file **Os/Task/Task.hpp** provides a base class definition of a registry
with the addTask() and removeTask() pure virtual function definitions.
Developers would write their own derived classes to implement a registry
that is appropriate for their system.

## Mutexes

The class definition can be found in **Os/Mutex.hpp**. The functions lock()
and unlock() lock and unlock the mutex. The mutex should be unlocked after
construction is complete.

## Message Queues

The class definition can be found in **Os/Queue.hpp**. This class implements
message queues. Table 24 describes the methods.

**Table 24.** Message queues method descriptions.

| Method       | Description                                                     |                                                                                                                                                                                                 |
| ------------ | --------------------------------------------------------------- | ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| create()     | Creates the message queue. The arguments are as follows:        |                                                                                                                                                                                                 |
|              | **Argument**                                                    | **Description**                                                                                                                                                                                 |
|              | name                                                            | A string identifying the queue. If the OS supports it, it will be used to name the queue.                                                                                                       |
|              | depth                                                           | The number of messages that a queue will support before dropping messages.                                                                                                                      |
|              | msgSize                                                         | Maximum size of a message.                                                                                                                                                                      |
|              | block                                                           | A flag to indicate whether or not message receive calls should block.                                                                                                                           |
| send()       | Send a message on the queue. The arguments are as follows:      |                                                                                                                                                                                                 |
|              | **Argument**                                                    | **Description**                                                                                                                                                                                 |
|              | buffer                                                          | Buffer to send. This version of the function takes a reference to a SerializeBufferBase instance. This is used by the framework to send serialized port calls.                                  |
|              | priority                                                        | The priority of the message.                                                                                                                                                                    |
| send()       | Send a message on the queue. The arguments are as follows:      |                                                                                                                                                                                                 |
|              | **Argument**                                                    | **Description**                                                                                                                                                                                 |
|              | buffer                                                          | Buffer to send. This version of the function takes a pointer to a byte buffer.                                                                                                                  |
|              | priority                                                        | The priority of the message. Depending on the underlying OS implementation, the messages are received in priority order.                                                                        |
| receive()    | Receive a message from the queue. The arguments are as follows: |                                                                                                                                                                                                 |
|              | **Argument**                                                    | **Description**                                                                                                                                                                                 |
|              | buffer                                                          | Serialized buffer to put received message in. This version of the function takes a reference to a SerializeBufferBase instance. This is used by the framework to receive serialized port calls. |
|              | priority                                                        | The priority of the received message.                                                                                                                                                           |
| receive()    | Receive a message from the queue. The arguments are as follows: |                                                                                                                                                                                                 |
|              | **Argument**                                                    | **Description**                                                                                                                                                                                 |
|              | buffer                                                          | Byte buffer to put received message in. This version of the function takes a pointer to a byte buffer.                                                                                          |
|              | capacity                                                        | The capacity of the receiving buffer. If it is not large enough for the message, the message will not be written and the function will return with an error.                                    |
|              | actualSize                                                      | The size of the received message. Will not exceed capacity.                                                                                                                                     |
|              | priority                                                        | The priority of the received message.                                                                                                                                                           |
| getNumQueues | Returns the number of message queues created in the system.     |                                                                                                                                                                                                 |

## Interval Timer

The class definition can be found in Os/IntervalTimer.hpp. An interval
timer is a facility that starts and stops the measurement of the passage of
time in the system. It is not an expiration timer that signals when it
is complete. It is used to measure execution times of, or to timestamp,
software activities. Table 25 provides the methods and their
descriptions.

**Table 25.** Interval timer method
descriptions.

| Method                | Description                                                                                          |
| --------------------- | ---------------------------------------------------------------------------------------------------- |
| start()               | Saves the start time of the timer.                                                                   |
| stop()                | Saves the stop time of the timer.                                                                    |
| getDiffUsec()         | Returns the time difference between start and stop in microseconds.                                  |
| getDiffUsec()         | Overloaded version that takes two times, subtracts them, and returns the difference in microseconds. |
| getRawTime()          | Gets the current raw time value. Can be used for time-tagging events.                                |

## Watchdog Timer

**Note:** This file implementation is not required for all OS adaptations.  Only those needing watchdog functionality.

The class definition can be found in **Os/WatchdogTimer.hpp** A watchdog
timer schedules a callback at the specified time in the future. It is a
one-shot timer; it needs to be rescheduled each time. Table 26 provides
the methods and their descriptions.

**Table 26.** Watchdog timer method descriptions.

| Method     | Description                                                                                                                                                     |                                                                                        |
| ---------- | --------------------------------------------------------------------------------------------------------------------------------------------------------------- | -------------------------------------------------------------------------------------- |
| startTicks | Starts the timer and gives the expiration in units of system ticks. The arguments are:                                                                          |                                                                                        |
|            | **Argument**                                                                                                                                                    | **Description**                                                                        |
|            | delayInTicks                                                                                                                                                    | Ticks to delay. OS/platform specific.                                                  |
|            | p\_callback                                                                                                                                                     | Function to call when timer expires.                                                   |
|            | parameter                                                                                                                                                       | Value passed to callback                                                               |
| startMs    | Starts the timer and gives the expiration in units of milliseconds. The arguments are:                                                                          |                                                                                        |
|            | **Argument**                                                                                                                                                    | **Description**                                                                        |
|            | delayInMs                                                                                                                                                       | Milliseconds to delay. Could be rounded up to the next system timer interval in ticks. |
|            | p\_callback                                                                                                                                                     | Function to call when timer expires.                                                   |
|            | parameter                                                                                                                                                       | Value passed to callback                                                               |
| restart()  | Restart the timer using the value passed to startTicks() or startMs(). If a different expiration time is desired, the start functions should be called instead. |                                                                                        |
| cancel()   | Cancel the timer in progress. The callback will not be called.                                                                                                  |                                                                                        |

## Interrupt Lock

The class definition can be found in Os/InterruptLock.hpp*.* An
interrupt lock prevents interrupts from preempting code execution. It
can be used as a very lightweight mutex on platforms that support
interrupt locking but should be used carefully as it is not subject to
priorities like a conventional mutex. In addition, it defers interrupts
that could be time critical, so the code executed between the locking
and unlocking should be very short in duration. This is a portable
interface, but the user should be aware of the behavior for each OS.
Table 27 provides the methods and their descriptions.

**Table 27.** Interrupt lock method descriptions.

| Method   | Description                                                                      |
| -------- | -------------------------------------------------------------------------------- |
| lock()   | Lock interrupts.                                                                 |
| unlock() | Unlock interrupts.                                                               |
| getKey() | Returns the interrupt lock key, if used by the OS. This is typically not needed. |

## File

The class definition can be found in Os/File.hpp*.* The file class
attempts to abstract the most common file functions to a class interface
to make porting code that does file accesses easier. Table 28 provides
the methods and their descriptions.

**Table 28.** File method descriptions.

| Method               | Description                                                                                                                                                         |                                                                                                                                                                        |
| -------------------- | ------------------------------------------------------------------------------------------------------------------------------------------------------------------- | ---------------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| open()               | Open the file with the given filename and mode.                                                                                                                     |                                                                                                                                                                        |
| isOpen()             | Detect if the file is open                                                                                                                                          |                                                                                                                                                                        |
| seek()               | Move the current location of the file to the offset. If absolute = true, move it relative to the beginning of the file, otherwise relative to the current location. |                                                                                                                                                                        |
| read()               | Reads data from the file into a buffer. The arguments are as follows:                                                                                               |                                                                                                                                                                        |
|                      | **Argument**                                                                                                                                                        | **Description**                                                                                                                                                        |
|                      | buffer                                                                                                                                                              | Destination buffer for data.                                                                                                                                           |
|                      | size                                                                                                                                                                | Size to read. When read is complete, size is modified to actual size.                                                                                                  |
|                      | waitForFull                                                                                                                                                         | If true, wait until full size is read, continuing reads when signals are encountered. If end-of-file is encountered, will return with less than full amount requested. |
| write()              | Writes data to the file from a buffer. The arguments are as follows:                                                                                                |                                                                                                                                                                        |
|                      | **Argument**                                                                                                                                                        | **Description**                                                                                                                                                        |
|                      | buffer                                                                                                                                                              | Source buffer for data.                                                                                                                                                |
|                      | size                                                                                                                                                                | Size to write. When write is complete, size is modified to actual size.                                                                                                |
|                      | waitForDone                                                                                                                                                         | If true, wait until full size is written, continuing writes when signals are encountered.                                                                              |
| flush()              | Flush data to disk. No-op on systems that do not support.                                                                                                           |                                                                                                                                                                        |
| close()              | Close the file. The file is automatically called by the destructor, but this method provides a way to manually close it as well.                                    |                                                                                                                                                                        |
| getLastError()       | Returns the last error value. Meant to abstract errno.                                                                                                              |                                                                                                                                                                        |
| getLastErrorString() | Returns a text description of the error for the last file operation. Meant to abstract strerror().                                                                  |                                                                                                                                                                        |
| calculateCRC32()     | Calculates the CRC32 of the file                                                                                                                                    |                                                                                                                                                                        |

## File System

The file system OS classes found in **Os/FileSystem.hpp** contain the file system helper calls to create directories, remove directories, etc.

**Note:** There are also the **Os/Directory.hpp** classes that allow users to stream directories.

## Log

This class definition can be found in **Os/Log.hpp**. It is an interface to
a system logging facility. It is meant to abstract the VxWorks logging
facility.  It is a subclass of `Fw::Logger` and thus must be registered after construction.  Compiling in
**Os/LogDefault.cpp** into your deployment will automatically create an **Os::Log** and register it.
Table 30 provides the methods and their descriptions.

**Table 30.** Log method descriptions.

| Method | Description                            |                                                                                                                                                                                                                                                                                                                                                                                                                                                                                          |
| ------ | -------------------------------------- | ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| logMsg | Log a text message. The arguments are: |                                                                                                                                                                                                                                                                                                                                                                                                                                                                                          |
|        | **Argument**                           | **Description**                                                                                                                                                                                                                                                                                                                                                                                                                                                                          |
|        | fmt                                    | Format string to fill and print.                                                                                                                                                                                                                                                                                                                                                                                                                                                         |
|        | a1 to a6                               | Values to be printed. They will be inserted into the format string based on the format string specifiers. The type of the argument is POINTER\_CAST (normally an integer), but a typical usage is to cast the values to display to POINTER\_CAST and allow the format string extraction to get the correct value. This can include strings (char\*), but the location in memory that holds the string must persist since the format string may be printed on another task in the system. |