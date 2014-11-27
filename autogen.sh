#!/bin/sh

#
# Boot autotools
#
echo "Boot autotools..."
autoreconf --install
automake --add-missing --copy
