# Autocoders Python Source 

## Autocoder tools

### codegen.py

Takes component, port type, serializable, topology, or enum XML files as input and produces FPrime Autocoded Ac.cpp/hpp files. Codegen is also set up to generate *Impl_[hpp,cpp] template implementation template files with -t/--template, unit test component files with -u/--unit-test, logger files through -l/--loggers and -L/--logger-output-file, dependency files through -d/--dependency-file, dictionary files through -g/--default_dict, -x/--xml_topology_dict, -T/--default_topology_dict, -a/--ampcs_dict, and -A/--ampcs_topology_dict, HTML docs through -H/--html_docs, Markdown docs through -m/--md_docs, and reports on component interfaces through -r/--gen_report.

### cosmosgen.py

Takes a topology XML file as input and generates all the configuration files necessary to add a deployment to Ball Aerospace's COSMOS suite of tools for sending and receiving commands and telemetry. The output directory defaults to $BUILD_ROOT/COSMOS. You can also delete a deployment from the COSMOS directory by using "-r DEPLOYMENT_NAME". Full documentation can be found in Autocoders/Python/src/fprime_ac/utils/cosmos/.

### JSONDictionaryGen.py

Takes a topology XML file as input and produces a JSON dictionary with naming convention "Deployment" + Dictionary.json. The JSON dictionary is generated within the directory where JSONDictionaryGen was run, but this can be overridden by inputting a new output directory in -p/--path.

### testgen.py



### gds_dictgen.py



### pymod_dictgen.py



### tlmLayout.py

Takes as input a telemetry layout in tab-delimited format and generates .hpp files of the packet layout(s) and a table that can be used to form the packet(s).

## Autocoders Python directory structure

### bin/

### doc/

### schema/

### src/

### templates/

### test/

### utils/

### CMakeLists.txt

### requirements.txt
