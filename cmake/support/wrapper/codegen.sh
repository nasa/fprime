#!/bin/bash
####
# Note:
#  This file wraps the code-generation call such that the following actions are performed:
#   1. Change directory to the Ai.xml source directory
#   2. Generate the Ac.cpp and Ac.hpp in place
#   3. Move them to the desired directory
#
# It takes 2 arguments: first, the path to the Ai.xml, second the directory where these files should live.
# The environment must include several settings. This can be done with the 'env' command in cmake.
#
# These envs include:
#   1. PYTHON_AUTOCODER_DIR: location of python auto coder directory
#   2. PYTHONPATH: python path setup for generating AC file
#   3. BUILD_ROOT: build root of leonardo
# Thus the run should look like:
#
#   cmake env -E PYTHON_AUTOCODER_DIR=<dir> BUILD_ROOT=<leonardo-fsw> PYTHONPATH=<path> codegen.sh <YES|NO> <Ai.xml> [<output, if not Ai.xml' dir>]
####
if [ $# -lt 2 ] || [ $# -gt 3 ]
then
    echo "[ERROR] Incorrect arguments.\nUsage:\n\t${0} <YES|NO> <Ai.xml> [<output dir>]"
    exit 1
elif [ ! -d ${3} ]
then
    echo "[ERROR] ${3} is not a directory."
    exit 2
elif [[ ${2} != *Ai.xml ]]
then
    echo "[ERROR] ${2} must be an Ai.xml file"
    exit 3
elif [[ ${1} != "YES" ]] && [[ ${1} != "NO" ]]
then
    echo "[ERROR] First argument must be 'YES' or 'NO'"
    exit 4
fi
# Helper variables
IS_TOP="${1}"
AI_XML=`basename $2`
AI_DIR=`dirname $2`
DEST_DIR="${AI_DIR}"
CPP=`echo ${AI_XML} | sed 's/Ai.xml/Ac.cpp/'`
HPP=`echo ${AI_XML} | sed 's/Ai.xml/Ac.hpp/'`
# Optional argument supplied, set move dir
if [ $# -eq 3 ]
then
    DEST_DIR="${3}"
fi
# Go to the AI directory
cd ${AI_DIR}
if [[ "${AI_XML}" == *Serializable* ]]
then
    ${PYTHON_AUTOCODER_DIR}/bin/codegen.py --build_root --default_topology_dict --dict_dir "Dict" "${AI_XML}"
elif [[ "${IS_TOP}" == "NO" ]]
then
    ${PYTHON_AUTOCODER_DIR}/bin/codegen.py --build_root "${AI_XML}"
else
    TMP_DIR="/tmp/serializables-$(date)"
    #NOTE: codegen will remove serializables sub directory. This moves them to safety and back
    mkdir -p "${TMP_DIR}"
    mv "${DICTIONARY_DIR}/serializable" "${TMP_DIR}/serializable"
    # TODO: update this for XML style dictionaries
    ${PYTHON_AUTOCODER_DIR}/bin/codegen.py --build_root --connect_only --default_topology_dict --dict_dir "${DICTIONARY_DIR}" "${AI_XML}"
    mv "${TMP_DIR}/serializable" "${DICTIONARY_DIR}/serializable"
    rmdir "${TMP_DIR}"
fi
# Move the output to the directory
CWD=`pwd`
if [[ ${CWD} != ${DEST_DIR} ]]
then
    mv ${CPP} ${DEST_DIR}
    mv ${HPP} ${DEST_DIR}
fi
