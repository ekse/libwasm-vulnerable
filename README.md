# libwasm

## Required packages:

* clang 4.0 or later
* libasan (AddressSanitizer)
* cmake

On Ubuntu:

    apt-get install clang-4.0 cmake libasan3

## Building the project (libFuzzer)

    mkdir -p builds/fuzzing
    cd builds/fuzzing
    CC=clang CXX=clang++ cmake -DWASM_FUZZING=1 ../..
    make

If using clang < 3.9.0, edit the main CMakeLists.txt to use "-fsanitize-coverage=bb" instead of
"fsanitize-coverage=trace-pc-guard"

  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fsanitize=address -fsanitize-coverage=trace-pc-guard")


## Building the project (AFL)

    mkdir builds
    mkdir builds/afl
    cd builds/afl
    env CC=../../afl-2.49b/afl-clang CXX=../../afl-2.49b/afl-clang++ cmake -DCMAKE_BUILD_TYPE=Debug ../..
    make

    cd ../../afl-2.49b
    ./afl-fuzz -i ../test_data/ -o test -- ../builds/afl/bin/wasm-dump @@


## Fuzzing with libFuzzer

Disable CPU scaling

    # as root
    cd /sys/devices/system/cpu
    echo performance | tee cpu*/cpufreq/scaling_governor

## Ubuntu 17.04 Docker

apt-get update
apt-get install clang-4.0 lldb-4.0 libasan3 make cmake nano
ln /usr/bin/clang-4.0 /usr/bin/clang
ln /usr/bin/clang++-4.0 /usr/bin/clang++
ln /usr/bin/lldb-4.0 /usr/bin/lldb
ln /usr/bin/llvm-symbolizer-4.0 /usr/bin/llvm-symbolizer

## Resources

http://llvm.org/docs/LibFuzzer.html
http://lcamtuf.coredump.cx/afl/README.txt
http://webassembly.org/docs/binary-encoding
