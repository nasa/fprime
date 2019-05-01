#!/bin/bash
DIRNAME="$(dirname $BASH_SOURCE)"
(
    cd "$DIRNAME/../../../cmake/test/src"
    pytest --ignore test_ut.py --ignore test_custom_target.py --ignore test_validation.py || exit 1
) || exit 2
