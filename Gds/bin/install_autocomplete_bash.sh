#!/bin/bash
COMMAND_NAME=$1
eval "$(register-python-argcomplete $COMMAND_NAME)"
