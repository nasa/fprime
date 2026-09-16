# RecordedCom - F' Subtopology

The starter files for the subtopology have been generated. To fully integrate it into your project, you need to do the following steps:

## 1. Add to Parent CMakeLists.txt
Add the `RecordedCom/` folder to its parent directory's `CMakeLists.txt` file if not already there:

```cmake
add_fprime_subdirectory("${CMAKE_CURRENT_LIST_DIR}/RecordedCom/")
```

## 2. Complete Your Subtopology Implementation
Update the following files based on your subtopology's requirements:

### Required Files:
- **`RecordedCom.fpp`**: Define your subtopology's instances and connections
- **`RecordedComConfig/RecordedComConfig.fpp`**: Configure constants, queue sizes, stack sizes, and priorities
- **`PingEntries.hpp`**: Define ping entries for health monitoring of components (if needed)

### Optional Files (add as needed):
- **`RecordedComConfig/RecordedComSubtopologyConfig.cpp/.hpp`**: 
  - Add C++ configuration code (e.g., memory allocators, custom initialization)
  - Update `RecordedComConfig/CMakeLists.txt` to include:
    ```cmake
    register_fprime_config(
        SOURCES
            "${CMAKE_CURRENT_LIST_DIR}/RecordedComSubtopologyConfig.cpp"
        HEADERS
            "${CMAKE_CURRENT_LIST_DIR}/RecordedComSubtopologyConfig.hpp"
        AUTOCODER_INPUTS
            "${CMAKE_CURRENT_LIST_DIR}/RecordedComConfig.fpp"
    )
    ```

- **`RecordedComConfig/Additional*.fpp`**: 
  - Add additional FPP configuration files (e.g., ComDriverConfig.fpp for communication subtopologies)
  - Include them in the `RecordedComConfig/CMakeLists.txt` AUTOCODER_INPUTS

- **`RecordedComTopologyDefs.hpp`**: 
  - Add topology state structures or additional definitions if needed
  - This file is already created but may need customization

## 3. Implementation Examples
Look at existing core subtopologies for reference:
- **ComFprime & ComCcsds**: Memory allocator configuration with C++ files, Communication driver configuration with additional FPP file
- **CdhCore**: Core command and datahandling functionality, Fault Handler configuration with additional FPP file 
- **FileHandling**: Simple configuration with only FPP files
- **DataProducts**: Data product handling
- **ComLoggerTee**: Instance-based subtopology (special case)

## 4. Common Patterns
- **Queue/Stack/Priority Configuration**: Define in `RecordedComConfig.fpp`
- **Memory Management**: Add SubtopologyConfig.cpp/.hpp for custom allocators
- **Topology Connections**: Define connections in the main `.fpp` file
- **Health Monitoring**: Update `PingEntries.hpp` with appropriate ping entries
- **Component Configuration**: Add additional FPP files in config `RecordedComConfig` 