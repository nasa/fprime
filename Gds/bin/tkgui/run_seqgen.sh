#!/bin/sh
# *******************************************************************************
# * Copyright 2013, by the California Institute of Technology.
# * ALL RIGHTS RESERVED. United States Government Sponsorship
# * acknowledged. Any commercial use must be negotiated with the Office
# * of Technology Transfer at the California Institute of Technology.
# *
# * This software may be subject to U.S. export control laws and
# * regulations. By accepting this document, the user agrees to comply
# * with all applicable U.S. export laws and regulations.  User has the 
# * responsibility to obtain export licenses,
# * or other export authority as may be required before exporting such
# * information to foreign countries or providing access to foreign
# * persons.
# *
DIRNAME="`dirname $0`"
"${DIRNAME}/run_tool.sh" seqgen "$@"
