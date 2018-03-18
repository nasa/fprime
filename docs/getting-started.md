---
title: Getting Started
---
## Getting Started with F'

The first step to running F' is to ensure that the required build tools are available on your system. At its most basic, F' requires
two tools to be installed: Git, GNU make, and python w/ cheetah. Make is available on most systems, and cheetah can be installed in python
with the following command:

```
pip install cheetah
```

### Cloning the Software and Building the Reference Application

The next step is to clone the software from GitHub. The following command will clone the software straight from NASA's GitHub project page.

```
git clone https://github.com/nasa/fprime.git
```

Once the software is cloned, the reference-application can be built by changing into the reference directory, generating make files,
clean and makeing the code.

```
cd fprime/Ref
make gen_make
make clean
make
```

If the build progresses to success, the reference application has been successfully built.
