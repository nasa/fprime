# Svc::ComSplitter

This is the build directory for the ComSplitter component.

## Redo Targets

**Main targets:** The following `redo` targets are available:

* `Dictionary`: Create a directory `Dictionary` containing a dictionary for the ISF Ground Support Equipment (GSE).

* `Docs`: Create a directory `Docs` containing ISF-style component documentation.

* `Interface`: Create a directory `Interface` containing a component interface.

* `NCSL`: Create a directory `NCSL` containing counts of non-commented source lines of code.

* `README`: Generate this `README` file.

* `Template`: Create a directory `Template` containing an ISF component template.

* `all`: Build the target `Build/$TARGET/$MODE/lib.a`, where `TARGET` and `MODE` are environment variables. The targets are Darwin, Linux, PI, and CORTEX160. The modes are Unit, Integration, and Flight. If `TARGET` is not set, the system will use the native environment (Darwin or Linux) as the default. If `MODE` is not set, the system will use Unit as the default.

* `clean`: Clean this directory and its subdirectories.

* `refresh`: Regenerate `ComSplitterComponentAi.xml` from `Model`.

**Helper targets:** The main targets use the following helper targets.
  You should not have to invoke these targets directly,
  except when developing or debugging the build system.

* `Table`: Create a directory `Table` containing the part of the table interface that is not represented in the ISF XML.

* `default.a`: Build a component library target `Build/`*target*`/`*mode*`/lib.a`.

* `default.ncsl.txt`: Generate an NCSL file.

* `default.o`: Build target of the form `Build/`*target*`/`*mode*`/`*path*`/`*file*`.o`, where *path* may be empty.
