To build these examples, from this directory:

    $ mkdir build
    $ cd build
    $ cmake ..
    $ make
    $ make install

This will compile and install the examples in a subdirectory of `build`, `build/installed`.

To install the examples elsewhere, you can replace `cmake ..` above with:

    $ cmake .. -DENABLE_INSTALL_PREFIX -DCMAKE_INSTALL_PREFIX="/usr/local/"

To specify a custom compiler:

    $ CXX="/usr/opt/gcc-4.8/bin/g++" cmake ..

