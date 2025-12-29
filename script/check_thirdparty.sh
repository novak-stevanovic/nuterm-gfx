#!/bin/bash

if ! pkgconf --exists nuterm; then
    echo "Missing dependency: nuterm - https://github.com/novak-stevanovic/nuterm"
    exit 1
fi

exit 0
