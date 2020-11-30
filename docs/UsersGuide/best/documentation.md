# Best Practice F´ Project Documentation

This document will walk through the recommended approach for documenting F´ projects.

F´ documents it's C++ code with [doxygen](www.doxygen.nl) and recommends users do the same. Doxygen
generates html documentation by extracting specially formatted comments within the source code. F´
has a Doxygen configuration file that can be used a base for user's own doxygen configuration at
`docs/doxygen/Doxyfile`.

To run doxygen on a F´ project, copy the F´ `Doxyfile` into the project, then run `doxygen <path to
Doxyfile>` in the root of your project.

To combine all the code documentation in one location, the F´ Doxyfile is also setup to embed markdown
files into the doxygen website, allowing component software design documents to be easily referenced
alongside the code documentation.

Take a look at the [F´ C++ API](../api/c++/html/index.html) to see doxygen based documentation in practice.
