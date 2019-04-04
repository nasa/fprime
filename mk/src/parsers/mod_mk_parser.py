import os.path
import parsers.variable_list_parser

# A lookup dictionary for setting up code generation rules.
# Key: Type of autocoded entity. Used to generate varibles, then list of code generation items
# First entry in list: Name of XML file suffix
# Second entry in list: List of suffixes of generated source files
# Third entry in list: List of suffixes of generated header files
# Fifth entry in list: make rule for generating code 

xml_gen_dictionary = {
                        'Serializable': 
                            [
                            'SerializableAi.xml',
                            ['SerializableAc.cpp',],
                            ['SerializableAc.hpp',],
                            '\tcd $(BUILD_ROOT)/<module_dir> && $(MKDIR) $(BUILD_ROOT)/<module_dir>/$(AC_DEP_DIR) && $(AC_SERIALIZABLE_GEN) $(notdir $<) $(DEP_FILE_ARG) $(BUILD_ROOT)/<module_dir>/$(AC_DEP_DIR)/$(basename $(notdir $<)).dep'
                            ],
                        'Port': 
                            [
                            'PortAi.xml',
                            ['PortAc.cpp',],
                            ['PortAc.hpp',],
                            '\tcd $(BUILD_ROOT)/<module_dir> && $(MKDIR) $(BUILD_ROOT)/<module_dir>/$(AC_DEP_DIR) && $(AC_INTERFACE_GEN) $(notdir $<) $(DEP_FILE_ARG) $(BUILD_ROOT)/<module_dir>/$(AC_DEP_DIR)/$(basename $(notdir $<)).dep'
                            ],
                        'Component': 
                            [
                            'ComponentAi.xml',
                            ['ComponentAc.cpp',],
                            ['ComponentAc.hpp',],
                            '\tcd $(BUILD_ROOT)/<module_dir> && $(MKDIR) $(BUILD_ROOT)/<module_dir>/$(AC_DEP_DIR) && $(AC_COMPONENT_GEN) $(notdir $<) $(DEP_FILE_ARG) $(BUILD_ROOT)/<module_dir>/$(AC_DEP_DIR)/$(basename $(notdir $<)).dep'
                            ],
                        'App': 
                            [
                            'AppAi.xml',
                            ['AppAc.cpp',],
                            ['AppAc.hpp',],
                            '\tcd $(BUILD_ROOT)/<module_dir> && $(MKDIR) $(BUILD_ROOT)/<module_dir>/$(AC_DEP_DIR) && $(AC_TOPOLOGY_GEN) $(notdir $<) $(DEP_FILE_ARG) $(BUILD_ROOT)/<module_dir>/$(AC_DEP_DIR)/$(basename $(notdir $<)).dep'
                            ],
                      }

# Some variable assignments to make reading the code easier
xml_source_file_list_entry = 0
xml_cpp_file_list_entry = 1
xml_hpp_file_list_entry = 2
xml_build_rule_list_entry = 3

lib_target = """
$(BUILD_ROOT)/<module_dir>/$(OUTPUT_DIR)/$(LIB_PREFIX)<module_name>$(LIB_SUFFIX): $(OBJS_<module_name>_$(BUILD)) $(OBJS_AC_<module_name>) 
	$(MKDIR) $(@D)
	echo "typedef int dummy;"> $(dir $@)empty.c
	$(CC) $(CFLAGS) $(COMPILE_ONLY) $(COMPILE_TO) $(dir $@)empty$(OBJ_SUFFIX) $(dir $@)empty.c
	$(LINK_LIB) $(LINK_LIB_FLAGS) $(LIBRARY_TO) $@ $^ $(dir $@)empty$(OBJ_SUFFIX)
	$(RM) $(dir $@)empty.c $(dir $@)empty$(OBJ_SUFFIX)
	$(POST_LINK_LIB) $(POST_LINK_LIB_ARGS) $@

# this is hard-coded for LARS NCSL. Fix in mk/src/parsers/mod_mk_parser.py.
<module_name>_sloc:
	@$(MKDIR) $(BUILD_ROOT)/<module_dir>/$(OUTPUT_DIR)
	@$(SLOC_COUNTER) $(SRC_<module_name>_$(BUILD)) $(HDR_<module_name>_$(BUILD)) > $(BUILD_ROOT)/<module_dir>/$(OUTPUT_DIR)/written_sloc.txt
	@$(SLOC_COUNTER) $(SRC_AC_<module_name>) $(HDR_AC_<module_name>) > $(BUILD_ROOT)/<module_dir>/$(OUTPUT_DIR)/ac_sloc.txt
	@$(SLOC_COUNTER) $(SRC_XML_AC_<module_name>) > $(BUILD_ROOT)/<module_dir>/$(OUTPUT_DIR)/xml_sloc.txt
	
<module_name>_sloc_dump:
	@echo "XML files:"
	@$(CAT) $(BUILD_ROOT)/<module_dir>/$(OUTPUT_DIR)/xml_sloc.txt
	@echo "Code-generated files:"
	@$(CAT) $(BUILD_ROOT)/<module_dir>/$(OUTPUT_DIR)/ac_sloc.txt
	@echo "Handcoded files:"
	@$(CAT) $(BUILD_ROOT)/<module_dir>/$(OUTPUT_DIR)/written_sloc.txt

""" 

test_bin_target = """
$(BUILD_ROOT)/<module_dir>/$(OUTPUT_DIR)/$(BIN_PREFIX)test_<last_dir>$(BIN_SUFFIX): $(TEST_MODS_LIBS_<module_name>_$(BUILD)) $(TEST_OBJS_<module_name>_$(BUILD))
	$(MKDIR) $(@D)
	$(LINK_LIB) $(LINK_LIB_FLAGS) $(LIBRARY_TO) $(basename $@)_test_lib$(LIB_SUFFIX) $(TEST_OBJS_<module_name>_$(BUILD))
	$(LINK_BIN) $(LINK_BIN_FLAGS) $(LINK_BIN_TO) $@ $(LIBS_START) $(basename $@)_test_lib$(LIB_SUFFIX) $(TEST_MODS_LIBS_<module_name>_$(BUILD)) $(TEST_LIBS_<module_name>_$(BUILD)) $(LINK_LIBS) $(LIBS_END) 

# clean unit test binary
test_<module_name>_clean: $(foreach module,$(TEST_MODS_<module_name>_$(BUILD)),$(module)_bin_clean)
	$(RM_DIR) $(TEST_OBJS_<module_dir>_$(BUILD)) $(BUILD_ROOT)/<module_dir>/$(OUTPUT_DIR)

"""

clean_target = """
<module_name>_clean: <module_name>_ac_clean <module_name>_bin_clean

<module_name>_ac_clean:
	$(RM) $(SRC_AC_<module_name>) $(HDR_AC_<module_name>) 
	$(RM_DIR) $(BUILD_ROOT)/<module_dir>/$(AC_DEP_DIR)
	$(RM_DIR) $(BUILD_ROOT)/<module_dir>/$(DICT_MODULE_SUBDIR)
	$(RM_DIR) $(BUILD_ROOT)/<module_dir>/$(AMPCS_DICT_MODULE_SUBDIR)
	
<module_name>_bin_clean:
	$(RM) $(OBJS_<module_name>_$(BUILD)) $(OBJS_AC_<module_name>)
	$(RM_DIR) $(BUILD_ROOT)/<module_dir>/$(OUTPUT_DIR) $(BUILD_ROOT)/<module_dir>/*/$(OUTPUT_DIR)
	
"""

dox_targets = """

gen_dox_<module_name>:
	cd $(BUILD_ROOT)/<module_dir> && $(DOXYGEN)

show_dox_<module_name>:
	$(BROWSER) $(BUILD_ROOT)/<module_dir>/dox/index.html

clean_dox_<module_name>:
	$(RM_DIR) $(BUILD_ROOT)/<module_dir>/dox
	
<module_name>_sdd: $(BUILD_ROOT)/<module_dir>/docs/sdd.html

$(BUILD_ROOT)/<module_dir>/docs/sdd.html: $(wildcard $(BUILD_ROOT)/<module_dir>/docs/sdd.md)
ifneq "$(wildcard $(BUILD_ROOT)/<module_dir>/docs/sdd.md)" ""
	@echo "Generating $@ from $<"
	@$(MKDIR) $(dir $@)
	@$(CAT) $(BUILD_ROOT)/mk/docs/SDD.css > $@
	-@$(MARKDOWN) $< >> $@
endif

<module_name>_sdd_clean:
	@$(RM) $(BUILD_ROOT)/<module_dir>/docs/sdd.html

"""

comp_helper_targets = """

<module_name>_testcomp:
	cd $(BUILD_ROOT)/<module_dir> && $(AC_TEST_COMPONENT_GEN) $(SRC_XML_AC_COMPONENT_<module_name>)

<module_name>_impl:
	cd $(BUILD_ROOT)/<module_dir> && $(AC_IMPL_GEN) $(SRC_XML_AC_COMPONENT_<module_name>)

"""

class CfgParseError(Exception):
    def __init__(self, error):
        self.error = error
    def getErr(self):
        return self.error
    
mod_make_cfg = "mod.mk"

class ModMkParser:
    
    
    def __init__(self, module, directory, isModule):
        self.source_dictionary = {} # holds source files by target
        self.hdr_dictionary = {} # holds header files by target
        self.test_source_dictionary = {} # holds test source by target
        self.xml_dictionary = {} # holds xml files  
        self.defines_dictionary = {} # holds extra defines by target
        self.post_defines_dictionary = {} # holds extra defines by target
        self.test_mods_dictionary = {} # hold module libraries to link test binary to 
        self.test_libs_dictionary = {} # hold libraries to link test binary
        self.ac_extra_list = [] # holds extra AC files
        
        self.subdir_parser_list = [] # holds a collection of parsers for each subdirectory 
        self.module_name = module
        self.module_local = os.path.split(directory)[1] # name of module local to directory; for finding files with module name as prefix
        # directory string is used for variable/target generation
        self.directory_string = directory.replace("/","_")
        self.directory = directory
        self.isModule = isModule # indicates whether this represents a module in the make system
        self.build_targets = ["",]

        # read build_targets
        build_file_parser = parsers.variable_list_parser.VariableListParser(os.environ["BUILD_ROOT"] + "/mk/configs/builds/builds.mk",":=")
        for build in build_file_parser.getValList("BUILDS"):
            self.build_targets += "_%s"%(build),
        # default to empty source lists so missing variables
        # are taken care of when makefile is generated.
        for target in self.build_targets:
            self.source_dictionary[target] = []
            self.hdr_dictionary[target] = []
            self.test_source_dictionary[target] = []
            self.test_libs_dictionary[target] = []
            self.test_mods_dictionary[target] = []
            self.defines_dictionary[target] = ""
            self.post_defines_dictionary[target] = ""

        for xml_type in list(xml_gen_dictionary.keys()):
            self.xml_dictionary[xml_type] = []
        # stored for debugging printout
        self.subdir_list = []
        
        if "PARSER_VERBOSE" in os.environ:
            print(("Analyzing directory " + directory))
        # check for file
        self.mod_file_name = directory + "/" + mod_make_cfg
        
        if not os.path.isfile(self.mod_file_name):
            raise CfgParseError("File %s does not exist." % self.mod_file_name)
        
        # read in file
        file_lines = open(self.mod_file_name).readlines()
        
        # for each line, search for variables.
        
        line_number = 1
        
        line = ""
        
        for curr_line in file_lines:
            # chop off comments (# character to end of page)
            # print("Processing line: " + line)
            comment_loc = curr_line.find("#")
            if comment_loc != -1:
                curr_line = curr_line[0:comment_loc]
            # strip off remaining whitespace
            curr_line = curr_line.strip()
            # check to see if empty line
            #print("Line length: " + str(len(line)))
            if len(curr_line) == 0:
                #print ("skipping")
                line_number += 1
                continue
            
            # look for continuation character
            cont_loc = curr_line.find("\\")
            if cont_loc != -1:
                curr_line = curr_line[0:cont_loc]
                curr_line = curr_line.strip()
                line += curr_line + " " # the space is for white space between file names
                line_number += 1
                continue
            else:
                line += curr_line + " "
            
            #print("Processed line: " + line)
            # split at first equal sign    
            # make sure remaining text has "=" in it somewhere
            # by searching for "="
            if line.count("=") < 1:
                raise CfgParseError("Invalid entry found in %s line %i" % (self.mod_file_name,line_number))
            (var,value) = line.split("=",1)
            var = var.strip()
            value = value.strip()
            
            # search for target specific variables
            for target in self.build_targets:

                starget = target

                if var == "SRC" + starget:
                    # make sure remaining text is of form "var = val"
                    # by searching for "="
                    if line.count("=") != 1:
                        raise CfgParseError("Invalid entry found in %s line %i" % (self.mod_file_name,line_number))
                    sources = value.split(" ")
                    for source in sources:
                        if source == "":
                            continue
                        if not os.path.isfile(os.environ["BUILD_ROOT"] + "/" + directory + "/" + source):
                            raise CfgParseError("File %s/%s/%s not found." % (os.environ["BUILD_ROOT"],directory,source))
                        if source.count(".xml"):
                            source_found = False
                            for xml_type in list(xml_gen_dictionary.keys()):
                                if source.count(xml_type):
                                    source_found = True
                                    self.xml_dictionary[xml_type] += source,
                            if not source_found:
                                source_names = ""
                                for xml_type in list(xml_gen_dictionary.keys()):
                                    source_names += "xx%sAi.xml "%xml_type
                                raise CfgParseError("File %s/%s/%s invalid. Should be one of \n\t%s" % (os.environ["BUILD_ROOT"],directory,source,source_names))
                                 
                        else:
                            self.source_dictionary[target] += source,
                            
                if var == "HDR" + starget:
                    # make sure remaining text is of form "var = val"
                    # by searching for "="
                    if line.count("=") != 1:
                        raise CfgParseError("Invalid entry found in %s line %i" % (self.mod_file_name,line_number))
                    headers = value.split(" ")
                    for header in headers:
                        if header == "":
                            continue
                        if not os.path.isfile(os.environ["BUILD_ROOT"] + "/" + directory + "/" + header):
                            raise CfgParseError("File %s/%s/%s not found." % (os.environ["BUILD_ROOT"],directory,header))
                        self.hdr_dictionary[target] += header,
                    
                                                
                if var == "TEST_SRC" + starget:
                    # make sure remaining text is of form "var = val"
                    # by searching for "="
                    if line.count("=") != 1:
                        raise CfgParseError("Invalid entry found in %s line %i" % (self.mod_file_name,line_number))
                    test_sources = value.split(" ")
                    for source in test_sources:
                        if source == "":
                            continue
                        if not os.path.isfile(os.environ["BUILD_ROOT"] + "/" + directory + "/" + source):
                            if not source.count("_ac_"): # Only disregard missing _AC_ files. For unit tests that want to include AC files that are not generated yet.
                                raise CfgParseError("File %s/%s/%s not found." % (os.environ["BUILD_ROOT"],directory,source))
                    self.test_source_dictionary[target] = test_sources
                    
                if var == "TEST_MODS" + starget:
                    if line.count("=") != 1:
                        raise CfgParseError("Invalid entry found in %s line %i" % (self.mod_file_name,line_number))
                    self.test_mods_dictionary[target] = value.split(" ")
                    
                if var == "TEST_LIBS" + starget:
                    if line.count("=") != 1:
                        raise CfgParseError("Invalid entry found in %s line %i" % (self.mod_file_name,line_number))
                    self.test_libs_dictionary[target] = value.split(" ")
                    
                if var == "COMPARGS" + starget:
                    self.defines_dictionary[target] = value
                        
                if var == "COMPARGS_POST" + starget:
                    self.post_defines_dictionary[target] = value
                        
            #  extra AC files
            
            if var == "AC_EXTRA_FILES":
                self.ac_extra_list = value.split(" ")

            # search for subdirectories
            
            if var == "SUBDIRS":
                
                # make sure remaining text is of form "var = val"
                # by searching for "="
                if line.count("=") != 1:
                    raise CfgParseError("Invalid entry found in %s line %i" % (self.mod_file_name,line_number))
                if value != "":
                    self.subdir_list = value.split(" ")
                
            line_number += 1
        
            # clear line accumulator
            line = ""
        # recurse into subdirectories    
        for subdir in self.subdir_list:
            if subdir == "":
                continue

            self.subdir_parser_list.append(ModMkParser(self.module_name+subdir, directory+"/"+subdir,False))
                
    def generateVariables(self, file_descriptor):
        
        # generate this module's variables
        if "PARSER_VERBOSE" in os.environ:
            print(("Generating variables for %s" % self.module_name))
        
        file_descriptor.write("\n# Module " + self.module_name + " make variables.\n")

        for target in list(self.source_dictionary.keys()):
            # write source variable
            file_descriptor.write("\nSRC_" + self.module_name + target + " := \\\n")
            # write each source    
            for source in self.source_dictionary[target]:
                if source != "":
                    file_descriptor.write("\t\t$(BUILD_ROOT)/" + self.directory + "/" + source + " \\\n")
                    # add source to sources to scan for EVRs
                
            # add variable for common source
            if target != "":
                file_descriptor.write("\t\t$(SRC_" + self.module_name + ")\n")

        for target in list(self.hdr_dictionary.keys()):
            # write header variable
            file_descriptor.write("\nHDR_" + self.module_name + target + " := \\\n")
            # write each header    
            for header in self.hdr_dictionary[target]:
                if header != "":
                    file_descriptor.write("\t\t$(BUILD_ROOT)/" + self.directory + "/" + header + " \\\n")
                
            # add variable for common header
            if target != "":
                file_descriptor.write("\t\t$(HDR_" + self.module_name + ")\n")


        for xml_type in list(self.xml_dictionary.keys()):
            
            file_descriptor.write("\nSRC_XML_AC_%s_%s := \\\n"%(xml_type.upper(),self.module_name))

            for source in self.xml_dictionary[xml_type]:
                # check to see if it is one of the special xml source files
                key_found = False
                for key in list(xml_gen_dictionary.keys()):
                    # print "Key: %s Replaced: %s" % (key,key.replace('%m',self.module_name))
                    if source.count(xml_gen_dictionary[key][xml_source_file_list_entry]):
                        file_descriptor.write("\t\t$(BUILD_ROOT)/%s/%s \\\n"%(self.directory,source))
                        key_found = True
                if not key_found:
                    raise CfgParseError("Invalid XML file %s in %s"%(source,self.mod_file_name))

            file_descriptor.write("\nSRC_AC_%s_%s := \\\n"%(xml_type.upper(),self.module_name))
        
            for source in self.xml_dictionary[xml_type]:
    
                # check to see if it is one of the special xml source files
                key_found = False
                for key in list(xml_gen_dictionary.keys()):
                    # print "Key: %s Replaced: %s" % (key,key.replace('%m',self.module_name))
                    if source.count(xml_gen_dictionary[key][xml_source_file_list_entry]):
                        key_found = True
                        for replacement in xml_gen_dictionary[key][xml_cpp_file_list_entry]:
                            gen_file = source.replace(xml_gen_dictionary[key][xml_source_file_list_entry],replacement)
                            file_descriptor.write("\t\t$(BUILD_ROOT)/" + self.directory + "/" + gen_file + " \\\n")
                if not key_found:
                    raise CfgParseError("Invalid XML file %s in %s"%(source,self.mod_file_name))

            file_descriptor.write("\nHDR_AC_%s_%s := \\\n"%(xml_type.upper(),self.module_name))

            for source in self.xml_dictionary[xml_type]:
                # check to see if it is one of the special xml source files
                for key in list(xml_gen_dictionary.keys()):
                    # print "Key: %s Replaced: %s" % (key,key.replace('%m',self.module_name))
                    if (source.count(xml_gen_dictionary[key][xml_source_file_list_entry])):
                        for replacement in xml_gen_dictionary[key][xml_hpp_file_list_entry]:
                            gen_file = source.replace(xml_gen_dictionary[key][xml_source_file_list_entry],replacement)
                            file_descriptor.write("\t\t$(BUILD_ROOT)/" + self.directory + "/" + gen_file + " \\\n")

        # add the XML sources together for tracking as a unit (e.g. for deletes/sloc)
        file_descriptor.write("\nSRC_XML_AC_%s := \\\n"%self.module_name)
        for xml_type in list(self.xml_dictionary.keys()):
            file_descriptor.write("\t\t$(SRC_XML_AC_%s_%s) \\\n"%(xml_type.upper(),self.module_name))
        
        # add the sources together for tracking them as a unit (e.g. for deletes/sloc
        file_descriptor.write("\nSRC_AC_%s := \\\n"%self.module_name)
        for xml_type in list(self.xml_dictionary.keys()):
            file_descriptor.write("\t\t$(SRC_AC_%s_%s) \\\n"%(xml_type.upper(),self.module_name))
            
        # add the headers together for tracking them as a unit (e.g. for deletes/sloc
        file_descriptor.write("\nHDR_AC_%s := \\\n"%self.module_name)
        for xml_type in list(self.xml_dictionary.keys()):
            file_descriptor.write("\t\t$(HDR_AC_%s_%s) \\\n"%(xml_type.upper(),self.module_name))

        # generate variable for object list for non-common targets
        for target in list(self.source_dictionary.keys()):
            if target != "":
                file_descriptor.write("\n\nOBJS_" + self.module_name + target + " := $(foreach source,$(SRC_" + self.module_name + target + "),$(dir $(source))$(OUTPUT_DIR)/$(OBJ_PREFIX)$(basename $(notdir $(source)))$(OBJ_SUFFIX))\n" )
                
        # generate variable for object list for XML targets
        for xml_type in list(self.xml_dictionary.keys()):
            file_descriptor.write("\nOBJS_AC_" + xml_type.upper() + "_" + self.module_name + " := $(foreach source,$(SRC_AC_" + xml_type.upper() + "_" + self.module_name + "),$(dir $(source))$(OUTPUT_DIR)/$(OBJ_PREFIX)$(basename $(notdir $(source)))$(OBJ_SUFFIX))\n" )
        
        # add the objects together for the library build
        file_descriptor.write("\nOBJS_AC_%s := \\\n"%self.module_name)
        for xml_type in list(self.xml_dictionary.keys()):
            file_descriptor.write("\t\t$(OBJS_AC_%s_%s) \\\n"%(xml_type.upper(),self.module_name))
        # generate test source. This could probably be refactored and combined with above, 
        # but this is the quickest way for now...
        for target in list(self.test_source_dictionary.keys()):
            # write source variable
            file_descriptor.write("\nTEST_SRC_" + self.module_name + target + " := \\\n")
            # write each source    
            for source in self.test_source_dictionary[target]:
                if source != "":
                    file_descriptor.write("\t\t$(BUILD_ROOT)/" + self.directory + "/" + source + " \\\n")
            # add variable for common source
            if target != "":
                file_descriptor.write("\t\t$(TEST_SRC_" + self.module_name + ")\n")    
                
        # generate variable for object list for non-common targets
        for target in list(self.test_source_dictionary.keys()):
            if target != "":
                file_descriptor.write("\nTEST_OBJS_" + self.module_name + target + " := $(foreach source,$(TEST_SRC_" + self.module_name + target + "),$(dir $(source))$(OUTPUT_DIR)/$(OBJ_PREFIX)$(basename $(notdir $(source)))$(OBJ_SUFFIX))\n" )


        # generate variable for object list for non-common targets
        for target in list(self.test_mods_dictionary.keys()):

            file_descriptor.write("\nTEST_MODS_LIBS_" + self.module_name + target + " := \\\n")
            # write each test module library    
            for mod in self.test_mods_dictionary[target]:
                if mod != "":
                    file_descriptor.write("\t\t$(BUILD_ROOT)/" + mod + "/$(OUTPUT_DIR)/$(LIB_PREFIX)" + mod.replace("/","") + "$(LIB_SUFFIX) \\\n")

            # add variable for common modules
            if target != "":
                file_descriptor.write("\t\t$(TEST_MODS_LIBS_" + self.module_name + ")\n")    

            file_descriptor.write("\nTEST_MODS_" + self.module_name + target + " := \\\n\t\t")
            # write each test module library    
            for mod in self.test_mods_dictionary[target]:
                if mod != "":
                    file_descriptor.write(mod.replace("/","") + " \\\n\t\t")

            # add variable for common modules
            if target != "":
                file_descriptor.write("\t\t$(TEST_MODS_" + self.module_name + ")\n")    

        for target in list(self.test_libs_dictionary.keys()):

            file_descriptor.write("\nTEST_LIBS_" + self.module_name + target + " := \\\n")
            # write each test module library    
            for lib in self.test_libs_dictionary[target]:
                if lib != "":
                    file_descriptor.write("\t\t" + lib + " \\\n")

            # add variable for common modules
            if target != "":
                file_descriptor.write("\t\t$(TEST_LIBS_" + self.module_name + ")\n")    
        
        for target in list(self.defines_dictionary.keys()):
            
            file_descriptor.write("\nDEFINES_" + self.directory_string + target + " := " + self.defines_dictionary[target] + " \\\n")

            # add variable for common modules
            if target != "":
                file_descriptor.write("\t\t$(DEFINES_" + self.directory_string + ")\n")    

        for target in list(self.post_defines_dictionary.keys()):
            
            file_descriptor.write("\nPOST_DEFINES_" + self.directory_string + target + " := " + self.post_defines_dictionary[target] + " \\\n")

            # add variable for common modules
            if target != "":
                file_descriptor.write("\t\t$(POST_DEFINES_" + self.directory_string + ")\n")    

        file_descriptor.write("\n-include $(BUILD_ROOT)/%s/extra_vars.mk\n\n" % self.directory)

        # generate source for any subdirectory parsers
        for parser in self.subdir_parser_list:
            parser.generateVariables(file_descriptor)

    def generateTargets(self, ac_file_descriptor, bin_file_descriptor):
        
        if "PARSER_VERBOSE" in os.environ:
            print(("Generating targets for %s" % self.module_name))
            
        ai_srcs = ""
        
        # generate this module's targets
        
        ac_file_descriptor.write("\n# Module " + self.module_name + " targets.\n\n")
        ac_file_descriptor.write("# source targets\n\n")
        bin_file_descriptor.write("\n# Module " + self.module_name + " targets.\n\n")
        bin_file_descriptor.write("# source targets\n\n")
        
        # if there are no sources for any targets, don't print the library target.
        files_visited = []
        
        # Only create regular targets for modules
        if (self.isModule):
                
            for target in list(self.source_dictionary.keys()):
    
                # write each source
                for source in self.source_dictionary[target]:
                    if source != "":
                                                                        
                        # convert from C/CPP file to object file
                        (path,source_file) = os.path.split(source)
                        if (path != ""):
                            path = "/" + path
                        if(files_visited.count(source) != 0):
                        # file already visited. skip
                            continue
                        files_visited.append(source)
                        
                        (base,extension) = os.path.splitext(source_file)
                        bin_file_descriptor.write("-include $(BUILD_ROOT)/" + self.directory + path + "/$(OUTPUT_DIR)/" + base + ".d\n")
                        bin_file_descriptor.write("$(BUILD_ROOT)/" + self.directory + path + "/$(OUTPUT_DIR)/$(OBJ_PREFIX)" + base + "$(OBJ_SUFFIX): $(BUILD_ROOT)/" + self.directory + "/" + source + "\n")
                        if (extension == ".c"):
                            bin_file_descriptor.write("\t$(MKDIR) $(@D)\n\t$(CC) -DASSERT_FILE_ID=$(shell $(FILE_HASH) $(notdir $<)) $(DEFINES_" + self.directory_string + "_$(BUILD)) $(DEPEND_FILE)$(basename $@).d $(CFLAGS) $(INCLUDE_PATH)$(dir $<) $(POST_DEFINES_" + self.directory_string + "_$(BUILD)) $(COMPILE_ONLY) $(COMPILE_TO) $@ $<\n\n")
                        elif ((extension == ".cpp") or (extension == ".cc") or (extension == ".asm")):
                            bin_file_descriptor.write("\t$(MKDIR) $(@D)\n\t$(CXX) -DASSERT_FILE_ID=$(shell $(FILE_HASH) $(notdir $<)) $(DEFINES_" + self.directory_string + "_$(BUILD)) $(DEPEND_FILE)$(basename $@).d $(CXXFLAGS) $(INCLUDE_PATH)$(dir $<) $(POST_DEFINES_" + self.directory_string + "_$(BUILD)) $(COMPILE_ONLY) $(COMPILE_TO) $@ $<\n\n")
                        else:
                            raise CfgParseError("Invalid Extension " + extension + " on file " + self.module_name + "/" + source)
    
            for xml_type in list(self.xml_dictionary.keys()):
    
                for source in self.xml_dictionary[xml_type]:
                        # check to see if it is one of the xml source files
                    for key in list(xml_gen_dictionary.keys()):
                        if source.count(xml_gen_dictionary[key][xml_source_file_list_entry]):
                            tgt_list = []
                            for replacement in xml_gen_dictionary[key][xml_cpp_file_list_entry]:
                                gen_file = source.replace(xml_gen_dictionary[key][xml_source_file_list_entry],replacement)
                                (source_file,extension) = os.path.splitext(gen_file)
                                # add dependency file inclusion
                                bin_file_descriptor.write("-include $(BUILD_ROOT)/" + self.directory + "/$(OUTPUT_DIR)/" + source_file + ".d\n")
                                bin_file_descriptor.write("$(BUILD_ROOT)/" + self.directory + "/$(OUTPUT_DIR)/$(OBJ_PREFIX)" + source_file + "$(OBJ_SUFFIX): $(BUILD_ROOT)/" + self.directory + "/" + gen_file + "\n")
                                if (extension == ".c"):
                                    bin_file_descriptor.write("\t$(MKDIR) $(@D)\n\t$(CC) $(INCLUDE_PATH)$(dir $<) -DASSERT_FILE_ID=$(shell $(FILE_HASH) $(notdir $<)) $(DEFINES_" + self.directory_string + "_$(BUILD)) $(DEPEND_FILE)$(basename $@).d $(CFLAGS) $(AC_CC_FLAGS) $(POST_DEFINES_" + self.directory_string + "_$(BUILD)) $(COMPILE_ONLY) $(COMPILE_TO) $@ $<\n\n")
                                elif ((extension == ".cpp") or (extension == ".cc")):
                                    bin_file_descriptor.write("\t$(MKDIR) $(@D)\n\t$(CXX) $(INCLUDE_PATH)$(dir $<) -DASSERT_FILE_ID=$(shell $(FILE_HASH) $(notdir $<)) $(DEFINES_" + self.directory_string + "_$(BUILD)) $(DEPEND_FILE)$(basename $@).d $(CXXFLAGS) $(AC_CXX_FLAGS) $(POST_DEFINES_" + self.directory_string + "_$(BUILD)) $(COMPILE_ONLY) $(COMPILE_TO) $@ $<\n\n")
                                else:
                                    raise CfgParseError("Invalid Extension " + extension + " on file " + self.module_name + "/" + gen_file)
                                tgt_list += "$(BUILD_ROOT)/%s/%s " % (self.directory,gen_file),
                                
                                
                            tgt_str = ""
                            for dep in tgt_list:
                                tgt_str += dep +" "
                                
                            # store input xml for sloc counter
                            ai_srcs += " $(BUILD_ROOT)/%s/%s"%(self.directory,source)
    
                            # write dependency include line
                            ac_file_descriptor.write("\n-include $(BUILD_ROOT)/%s/$(AC_DEP_DIR)/%s.dep"%(self.directory,os.path.splitext(os.path.basename(source))[0]))
                            ac_file_descriptor.write("\n%s: $(BUILD_ROOT)/%s/%s\n"%(tgt_str,self.directory,source))
                            ac_file_descriptor.write(xml_gen_dictionary[key][xml_build_rule_list_entry].replace('<module_dir>',self.directory) + "\n\n")
                                
            
            # write library target
            bin_file_descriptor.write("# library target\n")
            bin_file_descriptor.write(lib_target.replace("<module_dir>",self.directory).replace("<module_name>",self.module_name))
                
            # write clean target
            bin_file_descriptor.write(clean_target.replace("<module_dir>",self.directory).replace("<module_name>",self.module_name))
                    
            # write convenience target
            bin_file_descriptor.write("# convenience target\n\n")
            bin_file_descriptor.write(".PHONY: " + self.module_name + "\n")
            bin_file_descriptor.write(self.module_name + ": $(BUILD_ROOT)/" + self.directory + "/$(OUTPUT_DIR)/$(LIB_PREFIX)" + self.module_name + "$(LIB_SUFFIX)\n\n")
            
            ac_file_descriptor.write(comp_helper_targets.replace("<module_dir>",self.directory).replace("<module_name>",self.module_name))
            ac_file_descriptor.write(dox_targets.replace("<module_dir>",self.directory).replace("<module_name>",self.module_name))
    
            # test targets. Could probably be refactored to combine with above, but this is easier for now...    

        else: # Not a module; test targets are only in subdirectories
        
            bin_file_descriptor.write("\n# test file targets\n\n")    
    
            files_visited = []
            for target in list(self.test_source_dictionary.keys()):
                # write each source    
                for source in self.test_source_dictionary[target]:
                    if source != "":
                        # convert from C/CPP file to object file
                        (path,source_file) = os.path.split(source)
                        if (path != ""):
                            path = "/" + path
                        if (files_visited.count(source) != 0):
                            continue
                        files_visited.append(source)
                        
                        (base,extension) = os.path.splitext(source_file)
                        bin_file_descriptor.write("-include $(BUILD_ROOT)/" + self.directory + path + "/$(OUTPUT_DIR)/" + base + ".d\n")
                        bin_file_descriptor.write("$(BUILD_ROOT)/" + self.directory + path + "/$(OUTPUT_DIR)/$(OBJ_PREFIX)" + base + "$(OBJ_SUFFIX): $(BUILD_ROOT)/" + self.directory + "/" + source + "\n")
                        if (extension == ".c"):
                            bin_file_descriptor.write("\t$(MKDIR) $(@D)\n\t$(CC) -DASSERT_FILE_ID=$(shell $(FILE_HASH) $(notdir $<)) $(DEFINES_" + self.directory_string + "_$(BUILD)) $(DEPEND_FILE)$(basename $@).d $(CFLAGS) $(INCLUDE_PATH)$(dir $<) $(POST_DEFINES_" + self.directory_string + "_$(BUILD)) $(COMPILE_ONLY) $(COMPILE_TO) $@ $<\n\n")
                        elif ((extension == ".cpp") or (extension == ".cc")):
                            bin_file_descriptor.write("\t$(MKDIR) $(@D)\n\t$(CXX) -DASSERT_FILE_ID=$(shell $(FILE_HASH) $(notdir $<)) $(DEFINES_" + self.directory_string + "_$(BUILD)) $(DEPEND_FILE)$(basename $@).d $(CXXFLAGS) $(INCLUDE_PATH)$(dir $<) $(POST_DEFINES_" + self.directory_string + "_$(BUILD)) $(COMPILE_ONLY) $(COMPILE_TO) $@ $<\n\n")
                        else:
                            raise CfgParseError("Invalid Extension " + extension + " on file " + self.directory + "/" + source)
                                    
            # write binary/clean target
            bin_file_descriptor.write("# test binary target\n")
            #file_descriptor.write(test_bin_target % (self.directory, os.path.split(self.directory)[1], self.directory_string, self.directory_string, self.directory_string,self.directory_string,self.directory_string,self.directory_string)) 
            bin_file_descriptor.write(test_bin_target.replace("<module_dir>",self.directory).replace("<module_name>",self.module_name).replace("<last_dir>",os.path.split(self.directory)[1]))
                                
                                
                                
            # write convenience target
            bin_file_descriptor.write("# test convenience target\n\n")
            bin_file_descriptor.write("test_" + self.module_name + ": $(BUILD_ROOT)/" + self.directory + "/$(OUTPUT_DIR)/$(BIN_PREFIX)test_" + os.path.split(self.directory)[1] + "$(BIN_SUFFIX)\n\n")
    
            bin_file_descriptor.write("-include $(BUILD_ROOT)/%s/extra_rules.mk\n\n" % self.directory)
    
        # generate source for any subdirectory parsers
        for parser in self.subdir_parser_list:
            parser.generateTargets(ac_file_descriptor,bin_file_descriptor)
                            
