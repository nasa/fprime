# Defining Types 

## Background 

In F Prime, a **type definition** defines a kind of data that you can pass between components or use in commands and telemetry.

For this tutorial, you need one type definition. The type will define an enumeration called `MathOp`, which represents a mathematical operation.

## In this section 

## Setup 

To start, create a directory where your type(s) will live:

```shell
# In: MathProject 
mkdir -p Types 
cd Types
``` 

The user defines types in an fpp (F prime prime) file. Use the the command below to create an empty fpp file to define the `MathOp` type:

```shell 
# In: Types
touch MathTypes.fpp
```
Here you have created an empty fpp file named MathTypes in the Types directory.

## Implementing the Types 

Use your favorite text editor, visual studios, nano, vim, etc..., and add the following to `MathTypes.fpp`.

```
module MathModule{ 

    @ Math operations
    enum MathOp {
        ADD @< Addition
        SUB @< Subtraction
        MUL @< Multiplication
        DIV @< Division
  }
}
```
> Important note: think of modules similar to a cpp namespace. Whenever you want to make use of the enumeration, `MathOp`, you will need to use the MathModule module. 

Above you have created an enumation of the four math types that are used in this tutorial.

 
## Adding to the Build 

To specify how `MathTypes.fpp` should build with the project, you need to make two modifications to the MathProject. First, you must create and edit `CMakeLists.txt` in `Types` to include `MathTypes.fpp` into the build. Second, add the `Types` directory to the overall project build by adding to `project.cmake`. 

### First Modification 

To create CMakeLists.txt use:

```shell 
# In: Types
touch CMakeLists.txt 
```

> Note: capitalization and spelling is important when creating files!

Use a text editor to replace whatever is in CMakeLists.txt, most likely nothing, with the following.

```cmake 
set(SOURCE_FILES
  "${CMAKE_CURRENT_LIST_DIR}/MathTypes.fpp"
)

register_fprime_module()
```

### Second Modification 

Edit "project.cmake", located in the `MathProject` directory, and  add the following line. 

```cmake 
# In: MathProject/project.cmake
add_fprime_subdirectory("${CMAKE_CURRENT_LIST_DIR}/Types/")
```

The `Types` directory should now build without any issues. Test the build with the following commmands before moving forward.

```shell 
# In: Types 
fprime-util generate
fprime-util build 
```

## Summary 
In the section of the tutorial you have created the `Types` directory, `MathTypes.fpp`, defined one type, created a module, and added `Types` to the build. 

**Next:** [Constructing Ports](./constructing-ports.md)
