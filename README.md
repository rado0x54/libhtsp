# libhtsp a C++ library based on boost.

## Building
### CMake
To build the library run

    mkdir -p build && cd build && cmake .. && make

### Dependencies
- boost (system, asio)
Make sure that CMake can find [boost](http://www.boost.org)

- libhts
The project depends on `libhts` of the [pvr.hts](https://github.com/kodi-pvr/pvr.hts)
which is included as a git submodule. You may need to initialize it after cloning this
repository by running:

    git submodule update --init --remote --rebase --recursive

