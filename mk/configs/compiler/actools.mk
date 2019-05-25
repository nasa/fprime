#DICT_TYPE specifies what type of dictionary to generate, with the arguments being "PYTHON", "AMPCS", or "XML" 
#"PYTHON" is the default if the var is not set or is set to the wrong value. 
DICT_TYPE := "PYTHON"

# Python path addition for ACTOOLS

ifeq ("$(HOST)","Linux") # may need to set LXML_PATH and CHEETAH_PATH if they are somewhere other than where Python is installed
    export PYTHONPATH := $(BUILD_ROOT)/Autocoders/Python/src:$(BUILD_ROOT)/Autocoders/Python/utils::$(LXML_PATH):$(CHEETAH_PATH)
endif

ifeq ("$(HOST)","CYGWIN") # may need to set LXML_PATH and CHEETAH_PATH if they are somewhere other than where Python is installed
    export PYTHONPATH := $(BUILD_ROOT)/Autocoders/Python/src:$(BUILD_ROOT)/Autocoders/Python/utils:$(LXML_PATH):$(CHEETAH_PATH)
endif

ifeq ("$(HOST)","Darwin") # may need to set LXML_PATH and CHEETAH_PATH if they are somewhere other than where Python is installed
    export PYTHONPATH := $(BUILD_ROOT)/Autocoders/Python/src:$(BUILD_ROOT)/Autocoders/Python/utils::$(LXML_PATH):$(CHEETAH_PATH)
endif


CODE_GEN := $(PYTHON_BIN) $(BUILD_ROOT)/Autocoders/Python/bin/codegen.py
DICT_MODULE_SUBDIR := Dict
AMPCS_DICT_MODULE_SUBDIR := AMPCSDict

HTML_DOC_SUBDIR := docs
MD_DOC_SUBDIR := docs

AMPCS_DICT_DEPLOYMENT_SUBDIR := dict/AMPCS

AMPCS_COMMAND_MERGE := $(PYTHON_BIN) $(BUILD_ROOT)/Autocoders/Python/src/fprime_ac/converters/AmpcsCommandMerge.py
AMPCS_EVENT_MERGE := $(PYTHON_BIN) $(BUILD_ROOT)/Autocoders/Python/src/fprime_ac/converters/AmpcsEventMerge.py
AMPCS_TELEMETRY_MERGE := $(PYTHON_BIN) $(BUILD_ROOT)/Autocoders/Python/src/fprime_ac/converters/AmpcsTelemetryMerge.py

TELEMETRY_PACKETIZER := $(BUILD_ROOT)/mk/bin/run_tlm_packetizer.sh


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
    ifeq ($(DICT_TYPE) , "XML")
	   AC_SERIALIZABLE_GEN := $(CODE_GEN) --build_root
	   AC_TOPOLOGY_GEN := $(CODE_GEN) --build_root --connect_only --xml_topology_dict
	else
	   AC_SERIALIZABLE_GEN := $(CODE_GEN) --build_root --default_topology_dict --dict_dir $(DICT_MODULE_SUBDIR)
	   AC_TOPOLOGY_GEN := $(CODE_GEN) --build_root --connect_only  --default_topology_dict
    endif	
endif
	
AC_DEP_DIR	:= ac_dep

DEP_FILE_ARG := --dependency-file

COMP_REPORT_GEN := $(PYTHON_BIN) $(BUILD_ROOT)/mk/bin/comp_report_collate.py

AC_XML_VALIDATE = $(BUILD_ROOT)/mk/bin/run_xml_validate.sh
export AC_XML_VALIDATE
