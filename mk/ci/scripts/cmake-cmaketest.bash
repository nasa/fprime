#!/bin/bash
DIRNAME="$(dirname $BASH_SOURCE)"
(
    cd "$DIRNAME/../../../cmake/test/src"
    # Note: test_ut doesn't work inside a script, test_validation not finished
    pytest -v --ignore test_ut.py --ignore test_validation.py || exit 1
) || exit 2
