# Autocoders Python Source 

## Autocoder tools

### codegen.py

Takes component, port type, serializable, topology, or enum XML files as input and produces FPrime Autocoded Ac.cpp/hpp files. Codegen is also set up to generate *Impl_[hpp,cpp] template implementation template files with -t/--template, unit test component files with -u/--unit-test, logger files through -l/--loggers and -L/--logger-output-file, dependency files through -d/--dependency-file, dictionary files through -g/--default_dict, -x/--xml_topology_dict, -T/--default_topology_dict, -a/--ampcs_dict, and -A/--ampcs_topology_dict, HTML docs through -H/--html_docs, Markdown docs through -m/--md_docs, and reports on component interfaces through -r/--gen_report.

### cosmosgen.py

Takes a topology XML file as input and generates all the configuration files necessary to add a deployment to Ball Aerospace's COSMOS suite of tools for sending and receiving commands and telemetry. The output directory defaults to $BUILD_ROOT/COSMOS. You can also delete a deployment from the COSMOS directory by using "-r DEPLOYMENT_NAME". Full documentation can be found in Autocoders/Python/src/fprime_ac/utils/cosmos/.

### JSONDictionaryGen.py

Takes a topology XML file as input and produces a JSON dictionary with naming convention "Deployment" + Dictionary.json. The JSON dictionary is generated within the directory where JSONDictionaryGen was run, but this can be overridden by inputting a new output directory in -p/--path.

### testgen.py

Testgen generates a test component in the same way that codegen does. It takes a component XML file as input and generates Tester.cpp/hpp, TesterBase.cpp/hpp, GTestBase.cpp/hpp, and a TestMain.cpp. TestMain.cpp and Tester.cpp/hpp won't be overwritten if they already exists. Rather, one should use command line option -m/--maincpp to overwrite TestMain.cpp and -f/--force_tester to overwrite Tester.cpp and Tester.hpp. Testgen supports absolute paths to inputted files, so you are able to call testgen on any file and the test component will be generated in the directory that testgen was called from.

### gds_dictgen.py

GDS Dictgen is a tool for generating GDS XML Dictionaries that are named *TopologyAppDictionary.xml. The tool takes topology XML as input, and the generated dictionary contains all enums, serializables, commands, events, channels, and parameters present within the topology. GDS Dictgen supports absolute paths to inputted files, so you are able to call GDS dictgen on any file and the dictionary will be generated in the directory that GDS dictgen was called from.

### pymod_dictgen.py

Pymod Dictgen is a tool for generation python modules for the GDS. The tool takes topology XML as input, and it generates command, event, channel, and parameter python modules within their own respective directories. The output path can be changed with command line option -o/--dict_dir. Pymod dictgen supports absolute paths to inputted files, so you are able to call testgen on any file and the directories will be generated in the directory that pymod dictgen was called from.

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

Directory containing all autocoder RelaxNG and Schematron XML schemas. Schemas are found witin the default directory. The sample_XML_files directory contains sample XML files for the schemas that are run using the testSchemas.py test. These sample XML files were made for the RelaxNG schemas, but they have not been updated for the schematron schemas, so they may not work with them. 

### src/

Directory containing all python code for the autocoder tools. Codegen uses the visitor pattern to generate its output files, and the generators for the visitor pattern are found in generators while the visitors are found in generators/visitors. The directory generators/writers contain the modules that perform serial writes for the testgen and dictgen tools. They are alternatives to the visitors, but currently there are only writers for the test component and dictionary files. Generators/templates contains all the cheetah templates required to run the autocoder. In a fresh repo, these should be built either by making F Prime, making the autocoder unit tests, or manually running cheetah compile within the templates directory. The parsers directory contains all the custom autocoder XML parser modules that the autocoder uses to create models from raw F Prime XML. Lastly, utils/cosmos contains the modules used by the cosmosgen tool. There is a readme file within this directory that explains what all of this code does.

### templates/

Directory containing a sample unit test for testing components. There are sample F Prime XML files, a sample Impl.cpp file, and the ut itself.

### test/

Directory containing all unit tests invoked by the cmake system. List of working unit tests are present within CMakeLists.txt. The directories testgen, dictgen, schematron, and enum_xml contain pytests which should be run using pytest rather than cmake. Testgen and enum_xml additionally contain unit tests, but they are set up within cmake to be built as utilities rather than unit tests, so they will not be invoked when running all unit tests in cmake. Dependencies and file i/o within these unit tests prevent them from being successfully run outside of their pytests, so this should be kept this way.

### utils/

Contains a couple of nose tests which are no longer up-to-date.

### CMakeLists.txt

Cmake file that is present in all parts of directory tree to be built by cmake. If cmake was invoked with type testing, templates and test get built. The src directory is always built as well.

### requirements.txt

Up-to-date requirements file for using the autocoder with either Python2 and Python3. It can be run using "pip install -r ./requirements.txt"
