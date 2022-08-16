# FppTest/string

This directory contains unit tests for the FPP code generator.

* StringTest: Tests a nested string class within an array or struct class

To use this test suite, instantiate it with a list of types. For example, to
instantiate the test suite with two string types, `StringSize80` and 
`StringSize100`:

```c++
using StringTestImplementations = ::testing::Types<
    StringSize80,
    StringSize100
>;
INSTANTIATE_TYPED_TEST_SUITE_P(InstanceName, 
                               StringTest, 
                               StringTestImplementations);
```

And define an explicit specialization for the template function `getSize()` if
the string type does not have the default size of 80:

```c++
template<>
U32 getSize<StringSize100>() {
    return 100;
}
```
