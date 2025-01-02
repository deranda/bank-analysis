#!/bin/bash

#     ##########################################################################
#     <<<<<<<<<<<<<<<<<<<<<<<<<<< Definitions >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
#     ##########################################################################
PLUGIN_DEST_DIR="../../run/plugins"
LIBS_DEST_DIR="../../run/libs"
EXECUTABLE="../../run/backendCpp"
QT_PLUGIN_PATH="/opt/Qt/6.8.1/gcc_64/plugins"

# Check if QT_PLUGIN_PATH exists
if [ ! -d "$QT_PLUGIN_PATH" ]; then
  echo "Error: QT_PLUGIN_PATH ($QT_PLUGIN_PATH) does not exist."
  exit 1
fi
# Check if executable exists
if [ ! -f "$EXECUTABLE" ]; then
    echo "Error: Executable $EXECUTABLE not found!"
    exit 1
fi

#     ##########################################################################
echo "<<<<<<<<<<<<<<<<<<<<<<<<< Copying Libraries >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>"
#     ##########################################################################
# Create the libs directory if it doesn't exist
mkdir -p "$LIBS_DEST_DIR"

# Find and copy the dependencies
echo "Identifying dependencies for $EXECUTABLE..."
ldd "$EXECUTABLE" | awk '{ print $3 }' | while read -r LIB; do
    if [ -n "$LIB" ] && [ -f "$LIB" ]; then
        echo "Copying $LIB"
        cp -u "$LIB" "$LIBS_DEST_DIR/"
    fi
done

echo "All dependencies have been copied to $LIBS_DIR."

#     ##########################################################################
echo "<<<<<<<<<<<<<<<<<<<<<<<<<< Copying Plugins >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>"
#     ##########################################################################
BASE_DIR=$(dirname "$EXECUTABLE")  # The directory where the executable is located

# Create the plugins directory if it doesn't exist
mkdir -p "$PLUGIN_DEST_DIR"

echo "Running $EXECUTABLE under strace to identify plugins..."
strace -e openat "$EXECUTABLE" 2>&1 | grep "$QT_PLUGIN_PATH" > plugins.log

# Extract plugin paths and copy them
echo "Identifying and copying used plugins..."


grep -oE "$QT_PLUGIN_PATH/plugins/[a-zA-Z0-9_/.-]+" plugins.log | sort | uniq | while read -r PLUGIN; do
    PLUGIN_DIR=$(dirname "$PLUGIN")
    DEST_DIR="$PLUGIN_DEST_DIR/${PLUGIN_DIR##*/}"
    mkdir -p "$DEST_DIR"
    echo "Copying $PLUGIN to $DEST_DIR..."
    cp -u "$PLUGIN" "$DEST_DIR/"
done

echo "Required plugins have been copied to $PLUGIN_DEST_DIR."

