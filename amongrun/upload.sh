#!/usr/bin/env bash
set -e

#PORT=/dev/ttyUSB0
PORT=$(arduino-cli board list | awk '/ttyUSB|ttyACM/ {print $1; exit}')
FQBN=arduino:avr:nano:cpu=atmega328old
SKETCH=$1

arduino-cli compile --fqbn $FQBN "$SKETCH"
arduino-cli upload -p $PORT --fqbn $FQBN "$SKETCH"
