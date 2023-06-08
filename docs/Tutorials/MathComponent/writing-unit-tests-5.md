# Writing Unit Tests Part 5: Creating the Implementation Stub

## In this Section 

In this section of the tutorial, you will be repeating the steps 
you used to create an implementation stub for `MathSender`. 

In `Components/MathReceiver`, create a directory called UnitTests 

```shell 
# In: MathReceiver
mkdir -p UnitTests
```

Add the unit test to the build. Absolutely make sure that this is BELOW the existing stuff in the CMakeLists.txt:

```cmake 
# In: MathReceiver/CMakeLists.txt
# Below: register_fprime_module()

set(UT_SOURCE_FILES
  "${CMAKE_CURRENT_LIST_DIR}/MathReceiver.fpp"
)
set(UT_AUTO_HELPERS ON)
register_fprime_ut()
```

**Generate the unit test stub:**
Generate a stub implementation of the unit tests.


```shell 
# In: MathReceiver
fprime-util generate --ut 
fprime-util impl --ut
```
> These commands may take a while to run.

You haved just generate three new files `Tester.cpp Tester.hpp TestMain.cpp`. Move these files to the UnitTests directory in MathReceiver using:

```shell 
# In: MathReceiver
mv Tester.* TestMain.cpp UnitTests
```

Add `Tester.cpp and TestMain.cpp` to the build. Do so by editing the CMakeLists.txt in MathReceiver: 

```cmake
# In: MathReceiver/CMakeLists.txt 
set(UT_SOURCE_FILES
  "${CMAKE_CURRENT_LIST_DIR}/MathReceiver.fpp"
  "${CMAKE_CURRENT_LIST_DIR}/UnitTests/Tester.cpp"
  "${CMAKE_CURRENT_LIST_DIR}/UnitTests/TestMain.cpp"
)
set(UT_AUTO_HELPERS ON)
register_fprime_ut()
```

> Note: most of this is from a few steps ago, you will only be adding two lines in this step. 

Build the unit test in MathReceiver:

```shell 
# In: MathReceiver
fprime-util build --ut 
```
> Don't forget to add ```--ut``` or else you are just going to build the component again. 


## Summary

Here you setup a directory and build to start writing unit tests for 
`MathReceiver`. It is up to you to implement the ADD, SUB, MUL, and DIV
tests. The next section will work on building other types of tests.
Before moving onto the next section, do the following setup 
to prepare for random testing. 

```cpp
// In: Tester.cpp
#include "Tester.hpp"
#include "STest/Pick/Pick.hpp"
```

```cmake 
# In: /MathSender/CMakeLists.txt
set(UT_MOD_DEPS STest)
```

```cpp
// In: TestMain.cpp
#include "Tester.hpp"
#include "STest/Random/Random.hpp"
```

```cpp
// In: TestMain.cpp
STest::Random::seed();
```

**Next:** [Writing Unit Tests 6](./writing-unit-tests-6.md)
