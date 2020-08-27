# Svc::ComLogger Component

This is the build directory for the ISF ComLogger component.

## Redo Targets

**Main targets:** The following `redo` targets are available:

* `all`: Build the target `Build/$TARGET/$MODE/lib.a`, where `TARGET` and `MODE` are environment variables. The targets are Darwin, Linux, and CORTEX160. The modes are Unit, Integration, and Flight. If `TARGET` is not set, the system will use the native environment (Darwin or Linux) as the default. If `MODE` is not set, the system will use Unit as the default.

* `clean`: Clean this directory and its subdirectories.

* `Dictionary`: Create a directory `Dictionary` containing a dictionary for the ISF Ground Support Equipment (GSE).

* `Docs`: Create a directory `Docs` containing ISF-style component documentation.

* `html`: Generate `docs/ComLogger.html

* `Interface`: Create a directory `Interface` containing an ASTERIA-style component interface.

* `NCSL`: Create a directory `NCSL` containing counts of non-commented source lines of code.

* `README.md`: Generate this `README` file.

* `report`: Generate `ComLoggerComponentReport.txt`.

**Helper targets:** The main targets use the following helper targets.
  You should not have to invoke these targets directly,
  except when developing or debugging the build system.

* `default.a`: Build a component library target `Build/`*target*`/`*mode*`/lib.a`.

* `default.ncsl.txt`: Generate an NCSL file.

* `default.o`: Build target of the form `Build/`*target*`/`*mode*`/`*path*`/`*file*`.o`, where *path* may be empty.
