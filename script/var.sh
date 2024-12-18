#! /bin/bash
# DECLARE ENVIROMENT VARIABLES
SCRIPT_DIR="$(dirname "$(readlink -f "$0")")"

ZIG_PATH="$(dirname -- "$SCRIPT_DIR")/zig-linux-x86_64-0.14.0-dev.2487+af89bb05d"

PATH=$PATH:$ZIG_PATH

PROJECT_NAME=c_text_rpg

CC="zig cc"
CFLAGS="-std=c99 -Wall -Wextra -Wpedantic"
CSOURCE_FOLDER="$(dirname "$SCRIPT_DIR")/src"
CFILES="$(find "$CSOURCE_FOLDER" -name "*.c" | tr '\n' ' ')"

# Setup dirs
OUTPUT_DIR=$(dirname -- "$SCRIPT_DIR")/bin
LINUX_OUTPUT="$OUTPUT_DIR/$PROJECT_NAME"
WINDOWS_OUTPUT="$OUTPUT_DIR/$PROJECT_NAME.exe"
