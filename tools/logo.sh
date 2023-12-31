#!/bin/bash
set -e

CURRENT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"

DEST="$CURRENT_DIR/../overlay/data/logo_rgba.bin"
FONT="$CURRENT_DIR/../resources/FiraSans-Medium-rnx.ttf"
FONT_SIZE="30.5"
TEXT="sys-helo"

function render() {
	convert -background transparent -colorspace RGB -depth 8 -fill white -font "$1" -pointsize "$2" "label:$3" "$4"
}

render "$FONT" "$FONT_SIZE" "$TEXT" info:
render "$FONT" "$FONT_SIZE" "$TEXT" "RGBA:$DEST"
