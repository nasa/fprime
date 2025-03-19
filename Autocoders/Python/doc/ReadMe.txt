codegen.py Autocoder Tool to support F1 code generation
=======================================================
(Last updated 8 August 2013)

The codegen.py code generation produces four types of ISF codes.
These are: Component, Ports, Serializable, and Topology construction.
Each type of code has its own unique custom XML syntax. To date
there is no schema for the XML so users should use the examples
under the test folder for guidance.

codegen.py required two external Python packages to function,
which are lxml and cheetah.

To install lxml go to http://lxml.de/installation.html for download and
install instructions.   You might like to use the pip tool for installing
and a version is already in accurev and located at Autocoders/Python/utils/pip-1.2.1.
The other package you will need is the cheetah template language although I
think the yacgen.py will actually run without it.   Really Cheetah is just
used to generate the template Python.   I do not do the conversion at run-time
but precompile all templates and version control both templates and generated
python.  Anyway, documentation on Cheetah is located at
http://www.cheetahtemplate.org/index.html and one can download the
software from this site as well.   Cheetah is also in accurev and is
located at Autocoders/Python/utils/Cheetah-2.4.4.



