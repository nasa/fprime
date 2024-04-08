# Autocoders Python Source 

## Autocoder tools

### codegen.py
Takes component, port type, serializable, topology, or enum XML files as input and produces FPrime Autocoded Ac.cpp/hpp files. Codegen is also set up to generate *Impl_[hpp,cpp] template implementation template files with -t/--template, unit test component files with -u/--unit-test, logger files through -l/--loggers and -L/--logger-output-file, dependency files through -d/--dependency-file, dictionary files through -g/--default_dict, -x/--xml_topology_dict, -T/--default_topology_dict, -a/--ampcs_dict, and -A/--ampcs_topology_dict, HTML docs through -H/--html_docs, Markdown docs through -m/--md_docs, and reports on component interfaces through -r/--gen_report.

### cosmosgen.py
Takes a topology XML file as input and generates all the configuration files necessary to add a deployment to Ball Aerospace's COSMOS suite of tools for sending and receiving commands and telemetry. The output directory defaults to $BUILD_ROOT/COSMOS. You can also delete a deployment from the COSMOS directory by using "-r DEPLOYMENT_NAME". Full documentation can be found in Autocoders/Python/src/fprime_ac/utils/cosmos/.

### JSONDictionaryGen.py
Takes a topology XML file as input and produces a JSON dictionary with naming convention "Deployment" + Dictionary.json. The JSON dictionary is generated within the directory where JSONDictionaryGen was run, but this can be overridden by inputting a new output directory in -p/--path.

### testgen.py
Testgen generates a test component in the same way that codegen does. It takes a component XML file as input and generates Tester.cpp/hpp, TesterBase.cpp/hpp, GTestBase.cpp/hpp, and a TestMain.cpp. TestMain.cpp and Tester.cpp/hpp won't be overwritten if they already exist. Rather, one should use command line option -m/--maincpp to overwrite TestMain.cpp and -f/--force_tester to overwrite Tester.cpp and Tester.hpp. Testgen supports absolute paths to inputted files, so you are able to call testgen on any file and the test component will be generated in the directory that testgen was called from.

### gds_dictgen.py
GDS Dictgen is a tool for generating GDS XML Dictionaries that are named *TopologyAppDictionary.xml. The tool takes topology XML as input, and the generated dictionary contains all enums, serializables, commands, events, channels, and parameters present within the topology. GDS Dictgen supports absolute paths to inputted files, so you are able to call GDS dictgen on any file and the dictionary will be generated in the directory that GDS dictgen was called from.

### pymod_dictgen.py
Pymod Dictgen is a tool for generating python modules for the GDS. The tool takes topology XML as input, and it generates command, event, channel, and parameter python modules within their own respective directories. The output path can be changed with command line option -o/--dict_dir. Pymod dictgen supports absolute paths to inputted files, so you are able to call testgen on any file and the directories will be generated in the directory that pymod dictgen was called from.

### implgen.py
Implgen is a tool that takes in component XML and generates ComponentImpl.cpp/hpp files. It is formatted in a very similar way to testgen.py. It generates the Impl files within whatever directory it was ran, and it has its own pytest in Autocoders/Python/test/implgen. Implgen also allows for absolute paths.

### tlmLayout.py
Takes as input a telemetry layout in tab-delimited format and generates .hpp files of the packet layout(s) and a table that can be used to form the packet(s).

## Autocoders Python directory structure

### bin/
Directory containing all 7 autocoder tools and a few scripts to invoke them.

### doc/
Directory containing telemetry packet / layout documentation. It also contains a changelog and a readme for the autocoder, but both are outdated.

### schema/
Directory containing all autocoder RelaxNG and Schematron XML schemas. Schemas are found within the default directory. The sample_XML_files directory contains sample XML files for the schemas that are run using the testSchemas.py test. These sample XML files were made for the RelaxNG schemas, but they have not been updated for the schematron schemas, so they may not work with them. 

### src/
Directory containing all python code for the autocoder tools. Codegen uses the visitor pattern to generate its output files, and the generators for the visitor pattern are found in generators while the visitors are found in generators/visitors. Generators/templates contains all the cheetah templates required to run the autocoder. In a fresh repo, these should be built either by making F Prime, making the autocoder unit tests, or manually running cheetah compile within the templates directory. The parsers directory contains all the custom autocoder XML parser modules that the autocoder uses to create models from raw F Prime XML. Lastly, utils/cosmos contains the modules used by the cosmosgen tool. There is a readme file within this directory that explains what all of this code does.

#### src/generators/
| File | Description |
| ------------------------- | ---------------------------------------------------------------------------- |
| AbstractGenerator.py | Defines the interface for each portion or the code to be generated |
| ChannelBody.py | Main entry point of channel class body |
| ChannelHeader.py | Main entry point of channel class header |
| CommandBody.py | Main entry point of command class body  |
| CommandHeader.py | Main entry point of command class header |
| DictBody.py | Main entry point of dict class body |
| DictHeader.py | Main entry point of dict class header |
| DictStart.py | Main entry point of start of dict class code |
| EventBody.py | Main entry point of event class body |
| EventHeader.py | Main entry point of event class header |
| FinishSource.py | Main entry point of end of file code |
| formatters.py | Contains various routines to output formatted strings for code generation |
| GenFactory.py | Factory for instancing the interface and connecting appropriate visitors |
| HtmlDocPage.py | Main entry point for generation of html documentation for grnd interfaces |
| HtmlStartPage.py | Main entry point for generation of html documentation for grnd interfaces |
| Includes1.py | Main entry point for generation of a first set of include statements |
| Includes2.py | Main entry point for generation of a second set of include statements |
| InitFiles.py | Main entry point for generation of initializing code |
| InstanceDictBody.py | Main entry point for generation of instance dict body |
| InstanceDictHeader.py | Main entry point for generation of instance dict header |
| InstanceDictStart.py | Main entry point for generation of start of instance dict class code |
| MdDocPage.py | Main entry point of markdown documentation for ground interfaces |
| MdStartPage.py | Main entry point for start of code for markdown documentation |
| Namespace.py | Main entry point for generation of initialization code |
| Private.py | Main entry point for generation of private code within a class |
| Protected.py | Main entry point for generation of private code within a class |
| Public.py | Main entry point for generation of private code within a class |
| StartChannel.py | Main entry point for start of code for channel class |
| StartCommand.py | Main entry point for start of code for command class |
| StartEvent.py | Main entry point for start of code for event class |
| StartSource.py | Main entry point for start of code |

#### src/models/
| File | Description |
| --- | ----------- |
| Arg.py | Argument meta-model that is instanced as an association to port instances |
| Channel.py | Channel meta-model - list of channels |
| Command.py | Command meta-model - list of commands |
| CompFactory.py | Factory class for instancing the component and building port and arg config |
| Component.py | Component meta-model and main interface for code generation queries |
| Event.py | Event meta-model - list of event |
| InternalInterface.py | Internal interface meta-model - list of internal interfaces |
| ModelParser.py | Contains various routines for parsing the meta-model object and returning lists, dicts, etc. for visitors to utilize in mapping to code |
| Parameter.py | Parameter meta-model - list of parameters |
| Port.py | Port meta-model contained within a component class |
| PortFactory.py | Factory class for instancing the port interface type and building up port and arg config |
| Serialize.py | Serializable meta-model |
| TopoFactory.py | Factory class for instancing topology meta-model |
| Topology.py | Topology meta-model and main interface for code generation queries |

#### src/parsers/
| File | Description |
| --- | ----------- |
| AbstractParser.py | Defines the shared interfaces for parsing, validation and getter methods for component, port, and topology xml |
| XmlComponentParser.py | Parses XML component description files |
| XmlEnumParser.py | Parses XML enum description files |
| XmlParser.py | Parent class of the rest of the XmlParser classes |
| XmlPortsParser.py | Parses XML port description files |
| XmlSerializeParser.py | Parses XML serialize description files |
| XmlTopologyParser.py | Parses XML topology description files |

#### src/utils/
| File | Description |
| --- | ----------- |
| ac_cli_helpers.py | Contains helper methods to parse arguments and to run the autocoder CLI and pass any arguments it needs |
| AddSysPath.py | Contains a function that adds a directory to the Python sys.path value - copied from Python cookbook |
| CaltechHeader.py | Contains the caltech header that should be used in autocoder classes |
| ConfigManager.py | Simple configuration class patterned after the Keck observation sequencer GUI and the Tahoe CalVal pipeline |
| DictTypeConverter.py | Contains methods to convert types to dictionary types and to replace special characters in format strings |
| DiffAndRename.py | Mainly used for difference and rename routines |
| DumpObj.py | Contains methods to print nicely formatted overviews of objects |
| EnumDictCheck.py | A structure used to report enumeration size errors |
| EnumGenerator.py | Generator to produce serializable enums |
| Logger.py | Sets up the logging for all other scripts based on the Python logging module - not a standalone file |
| ParseC.py | Contains a set of Python functions that parse C code |
| pyparsing.py | Python parser methods created externally by Paul T. McGuire |
| TopDictGenerator.py | Generator that produces a topology dictionary from parsed XML |
| TypesList.py | Contains types_list and port_types_list which contain types allowed in commands/telemetry/parameters/events |
| VersionFile.py | Check if file exists, and creates a backup copy using an appended versioning string - copied directly from the language cookbook |
| XmlParser.py | General XML parser class based on the Python xml2obj recipe |

### templates/
Directory containing a sample unit test for testing components. There are sample F Prime XML files, a sample Impl.cpp file, and the ut itself.

### test/
Directory containing all unit tests invoked by the cmake system. List of working unit tests are present within CMakeLists.txt. The directories testgen, dictgen, schematron, and enum_xml contain pytests which should be run using pytest rather than cmake. Testgen and enum_xml additionally contain unit tests, but they are set up within cmake to be built as utilities rather than unit tests, so they will not be invoked when running all unit tests in cmake. Dependencies and file i/o within these unit tests prevent them from being successfully run outside of their pytests, so this should be kept this way.

### CMakeLists.txt
Cmake file that is present in all parts of directory tree to be built by cmake. If cmake was invoked with type testing, templates and test get built. The src directory is always built as well.

### Requirements

The Autocoder's requirements are covered by installing the F´ software package. This is covered by the install document 
found at: [INSTALL.md](../../docs/INSTALL.md).

## Schematron
In Autocoders/Python/schema/default there are various schematron .rng files mixed with the normal F Prime RelaxNG .rng schema files. The schematron files validate xml in the same way as the RelaxNG schemas (within the XmlParser classes), but these schematron files will only generate command line errors, not exceptions like the schema files will. Here is a short description of each file:

| Schematron File | Description |
| ------------------------- | ---------------------------------------------------------------------------- |
| top_uniqueness_schematron.rng | Compares base_id attributes of all instance xml elements by checking that the values within their base_id_window don't overlap  |
| active_comp_schematron.rng | Checks that all active component xml elements have a child port element that is of type async_input |
| enum_value_schematron.rng | Checks that if one enum item xml element has an attribute "value" then all enum item xml elements have an attribute "value". It also checks uniqueness of these values |
| comp_uniqueness_schematron.rng | Checks that all ID's and opcodes for their respective elements within a component xml element are unique - this is the same as the bottom four schematron files but this file only runs on component xml tags  |
| command_op_schematron.rng | Checks uniqueness of command opcodes in command xml elements  |
| event_id_schematron.rng | Checks uniqueness of event id's in event xml elements  |
| channel_id_schematron.rng | Checks uniqueness of channel id's in channel xml elements  |
| parameter_id_schematron.rng | Checks uniqueness of parameter id's in parameter xml elements  |

### Schematron Syntax
The root element of a schematron file is a schema tag with attribute xmlns set to "http://purl.oclc.org/dsdl/schematron". You are able to define any number of patterns with any number of rules within these files, where a pattern is a set of rules and a rule defines a set of asserts that all run on an xml element where the element's name is defined in the rule's context attribute. Schematron asserts use XPath syntax. It is helpful to find XPath cheat sheets online and have them open when defining a schematron assert.
