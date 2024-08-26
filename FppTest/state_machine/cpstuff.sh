#!/bin/bash

# Define the source and destination directories
SOURCE_DIR="/home/watney/fprime/FppTest/state_machine/dev"
DEST_DIR="/home/watney/fprime/FppTest/build-fprime-automatic-native-ut/FppTest/state_machine"

# List of files to copy
FILES=("SmTestComponentAc.cpp" "SmTestComponentAc.hpp")

# Loop through each file and copy it to the destination directory
for FILE in "${FILES[@]}"; do
  if [ -f "$SOURCE_DIR/$FILE" ]; then
    cp "$SOURCE_DIR/$FILE" "$DEST_DIR/"
    echo "Copied $FILE to $DEST_DIR/"
  else
    echo "File not found: $SOURCE_DIR/$FILE"
  fi
done

echo "File copy operation completed."

