#! /bin/bash

aclocal
libtoolize
autoreconf
automake --add-missing
autoreconf
