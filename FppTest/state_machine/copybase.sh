#!/bin/bash

DESTINATION_DIR="../build-fprime-automatic-native-ut/FppTest/state_machine"

# Copy the file to the destination with a new name
cp "SmTestComponent.hpp" "$DESTINATION_DIR/SmTestComponentAc.hpp"
cp "SmTestComponent.cpp" "$DESTINATION_DIR/SmTestComponentAc.cpp"

