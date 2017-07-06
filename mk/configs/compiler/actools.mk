# ACTools script definitions
#DICT_GEN specifies if instances of components should be specified in AcConstants.ini ("LOCAL") or should be drawn from the
#Topology XML files ("GLOBAL").
#If "GLOBAL" is used, please zero out all IDS by replacing them with 0x000.
#"LOCAL" is the default if this var is not set or is not set to "GLOBAL" or "LOCAL".
DICT_GEN := "GLOBAL"

#DICT_TYPE specifies what type of dictionary to generate, with the two arguments being "DEFAULT" or "AMPCS". 
#"DEFAULT" is the default if the var is not set or is set to the wrong value. 
DICT_TYPE := "DEFAULT"

# Python path addition for ACTOOLS

ifeq ("$(HOST)","Linux") # may need to set LXML_PATH and CHEETAH_PATH if they are somewhere other than where Python is installed
    export PYTHONPATH := $(BUILD_ROOT)/Autocoders/src:$(BUILD_ROOT)/Autocoders/utils::$(LXML_PATH):$(CHEETAH_PATH)
endif

ifeq ("$(HOST)","CYGWIN") # may need to set LXML_PATH and CHEETAH_PATH if they are somewhere other than where Python is installed
    export PYTHONPATH := $(BUILD_ROOT)/Autocoders/src:$(BUILD_ROOT)/Autocoders/utils:$(LXML_PATH):$(CHEETAH_PATH)
endif

ifeq ("$(HOST)","Darwin") # may need to set LXML_PATH and CHEETAH_PATH if they are somewhere other than where Python is installed
    export PYTHONPATH := $(BUILD_ROOT)/Autocoders/src:$(BUILD_ROOT)/Autocoders/utils::$(LXML_PATH):$(CHEETAH_PATH)
endif


CODE_GEN := $(PYTHON_BIN) $(BUILD_ROOT)/Autocoders/bin/codegen.py
DICT_MODULE_SUBDIR := Dict
AMPCS_DICT_MODULE_SUBDIR := AMPCSDict

HTML_DOC_SUBDIR := docs
MD_DOC_SUBDIR := docs

AMPCS_DICT_DEPLOYMENT_SUBDIR := dict/AMPCS

AMPCS_COMMAND_MERGE := $(PYTHON_BIN) $(BUILD_ROOT)/Autocoders/src/converters/AmpcsCommandMerge.py
AMPCS_EVENT_MERGE := $(PYTHON_BIN) $(BUILD_ROOT)/Autocoders/src/converters/AmpcsEventMerge.py
AMPCS_TELEMETRY_MERGE := $(PYTHON_BIN) $(BUILD_ROOT)/Autocoders/src/converters/AmpcsTelemetryMerge.py


ifeq ($(DICT_GEN),"GLOBAL")
	AC_INTERFACE_GEN := $(CODE_GEN) --build_root
	# DEFAULT Dictionary
	AC_COMPONENT_GEN := $(CODE_GEN) --build_root --default_topology_dict --gen_report --html_docs --html_doc_dir $(HTML_DOC_SUBDIR) --md_docs --md_doc_dir $(MD_DOC_SUBDIR)
	#
	AC_TEST_COMPONENT_GEN := $(CODE_GEN) --build_root -u
	#
	AC_IMPL_GEN := $(CODE_GEN) --build_root -t
	# AMPCS Dictionary
ifeq ($(DICT_TYPE) , "AMPCS")
	AC_SERIALIZABLE_GEN := $(CODE_GEN) --build_root
	AC_TOPOLOGY_GEN := $(CODE_GEN) --build_root --connect_only  --ampcs_topology_dict --dict_dir $(AMPCS_DICT_MODULE_SUBDIR)
else
	AC_SERIALIZABLE_GEN := $(CODE_GEN) --build_root --default_topology_dict --dict_dir $(DICT_MODULE_SUBDIR)
	AC_TOPOLOGY_GEN := $(CODE_GEN) --build_root --connect_only  --default_topology_dict
endif
	
else # local
	AC_SERIALIZABLE_GEN := $(CODE_GEN) --default_dict --dict_dir $(DICT_MODULE_SUBDIR) --build_root 
	AC_INTERFACE_GEN :=  $(CODE_GEN) --build_root
	
ifeq ($(DICT_TYPE) , "AMPCS")
	# AMPCS Dictionary
	AC_COMPONENT_GEN :=  $(CODE_GEN) --build_root --ampcs_dict --gen_report --dict_dir $(AMPCS_DICT_MODULE_SUBDIR) \
					--html_docs --html_doc_dir $(HTML_DOC_SUBDIR)  --md_docs --md_doc_dir $(MD_DOC_SUBDIR)
else
	# DEFAULT Dictionary
	AC_COMPONENT_GEN :=  $(CODE_GEN) --default_dict--dict_dir $(DICT_MODULE_SUBDIR) --build_root --gen_report \
					--html_docs --html_doc_dir $(HTML_DOC_SUBDIR) --md_docs --md_doc_dir $(MD_DOC_SUBDIR)
endif
	AC_TEST_COMPONENT_GEN :=  $(CODE_GEN) --build_root -u

	AC_IMPL_GEN := $(CODE_GEN) --build_root -t


	AC_TOPOLOGY_GEN := $(CODE_GEN) --build_root --connect_only

endif
	


AC_DEP_DIR	:= ac_dep

DEP_FILE_ARG := --dependency-file

COMP_REPORT_GEN := $(PYTHON_BIN) $(BUILD_ROOT)/mk/bin/comp_report_collate.py

AC_XML_VALIDATE = $(BUILD_ROOT)/mk/bin/run_xml_validate.sh
export AC_XML_VALIDATE
