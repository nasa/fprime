# macOS

macOS, like Linux, is a Unix system and thus may be used directly for most of this 
tutorial. However, Mac users must install the following utilities 
*and ensure they are available on the command line path*.

1. [Python 3](https://www.python.org/downloads/release/python-3913/)
2. [CMake](https://cmake.org/download/)
3. GCC/CLang typically installed with xcode-select

**Installing GCC/CLang on macOS**
```bash
xcode-select --install
```

Installing Python and running the above command to install gcc/CLang should ensure 
that those tools are on the path.

CMake requires one additional step to ensure it is on the path:

```bash
sudo "/Applications/CMake.app/Contents/bin/cmake-gui" --install
```

In order to cross-compile, a Linux box is essential. You may choose to use a virtual 
machine or may choose to follow the instructions in [Appendix I](./appendix-1.md) to 
install a docker container including the necessary tools. 

After the steps above are completed, [return to the tutorial.](../CrossCompilationSetup/CrossCompilationSetupTutorial.md)