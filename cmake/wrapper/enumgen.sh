#!/bin/bash
####
# Note:
####
if [ $# -lt 1 ] || [ $# -gt 2 ]
then
    echo "[ERROR] Incorrect arguments.\nUsage:\n\t${0} <Ai.txt> [<output dir>]"
    exit 1
elif [ ! -d ${2} ]
then
    echo "[ERROR] ${2} is not a directory."
    exit 2
elif [[ ${1} != *Ai.txt ]]
then
    echo "[ERROR] ${1} must be an Ai.txt file"
    exit 3
fi
# Helper variables
AI_TXT=`basename $1`
AI_DIR=`dirname $1`
DEST_DIR="${AI_DIR}"
CPP=`echo ${AI_TXT} | sed 's/Ai.txt/Ac.cpp/'`
HPP=`echo ${AI_TXT} | sed 's/Ai.txt/Ac.hpp/'`
# Optional argument supplied, set move dir
if [ $# -eq 2 ]
then
    DEST_DIR="${2}"
fi
# Go to the AI directory
cd ${AI_DIR}
${SHELL_AUTOCODER_DIR}/bin/enum_cpp.sh ${AI_TXT}
${SHELL_AUTOCODER_DIR}/bin/enum_hpp.sh ${AI_TXT}
# Move the output to the directory
CWD=`pwd`
if [[ ${CWD} != ${DEST_DIR} ]]
then
    mv ${CPP} ${DEST_DIR}
    mv ${HPP} ${DEST_DIR}
fi
