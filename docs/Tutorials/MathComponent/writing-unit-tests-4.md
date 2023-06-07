# Writing Unit Tests Part 2: Random testing

## Background 

Testing using only numbers that you hard code into your tests can easily leave edge cases untouched can allow you, the programmer, to miss bugs.

F Prime provides a module called STest that provides helper classes and functions for writing unit tests. As an exercise, use the interface provided by STest/STest/Pick.hpp to pick random values to use in the tests instead of using hard-coded values such as 2.0, 3.0, and 10.

## In this Section

In this section of the tutorial, you will create test that uses random numbers instead of hard coded numbers. 

To incorporate random numbers into the existing tests you have written for `MathSender`, you only need to make a couple small modifications. 

First, edit `MathSender/UnitTests/Tester.cpp` by adding a `Pick.hpp` to the includes: 

```cpp
// In: Tester.cpp
#include "Tester.hpp"
#include "STest/Pick/Pick.hpp"
```

Second, modify `MathSender/CMakeLists.txt` to include STest as a build dependancy:

```cmake 
# In: /MathSender/CMakeLists.txt
set(UT_MOD_DEPS STest)
```

Third, modify `TestMain.cpp` to include `Random.hpp`:

```cpp
// In: TestMain.cpp
#include "Tester.hpp"
#include "STest/Random/Random.hpp"
```


Fourth, add the following line to the main function of `TestMain.cpp`, just before the return statement:

```cpp
// In: TestMain.cpp
STest::Random::seed();
```
**Run the Test:**
Recompile and rerun the tests:

```shell
# In: MathSender  
fprime-util build --ut 
fprime-util check 
```

 Now go to MathProject/build-fprime-automatic-native-ut/Components/MathSender and inspect the file `seed-history`. This file is a log of random seed values. Each line represents the seed used in the corresponding run.

**Fixing the Random Seed:**
Sometimes you may want to run a test with a particular seed value, e.g., for replay debugging. To do this, put the seed value into a file `seed` in the same directory as `seed-history`. If the file seed exists, then STest will use the seed it contains instead of generating a new seed.

Try the following:

   1. Copy the last value S of ```seed-history``` into ```seed```.

   2. In Components/MathSender, re-run the unit tests a few times.

   3. Inspect ```MathProject/build-fprime-automatic-native-ut/Components/MathSender/seed-history```. You should see that the value S was used in the runs you just did (corresponding to the last few entries in seed-history).

## Summary 

In this section you incorprated random testing into your existing tests. You used random

**Next:** [Writing Unit Tests 5](./writing-unit-tests-5.md)