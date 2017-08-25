# libwasm

## Required packages:

* clang 4.0 or later
* libasan (AddressSanitizer)
* cmake

On Ubuntu:

    apt-get install clang-4.0 cmake libasan3

## Ubuntu 17.04 Docker
    
    docker pull ubuntu:zesty
    docker run --rm -it --security-opt seccomp=unconfined ubuntu:zesty bash

    apt-get update
    apt-get install clang-4.0 lldb-4.0 libasan3 make cmake nano git
    ln /usr/bin/clang-4.0 /usr/bin/clang
    ln /usr/bin/clang++-4.0 /usr/bin/clang++
    ln /usr/bin/lldb-4.0 /usr/bin/lldb
    ln /usr/bin/llvm-symbolizer-4.0 /usr/bin/llvm-symbolizer
    
    git clone https://github.com/ekse/libwasm-vulnerable/
    
    # build afl
    cd afl-2.49b
    make
    cd ..
    
    # build libfuzzer
    cd libfuzzer
    ./build.sh
    cp libFuzzer.a ..
    cd ..


## Building the project (libFuzzer)

    mkdir -p builds/fuzzing
    cd builds/fuzzing
    CC=clang CXX=clang++ cmake -DWASM_FUZZING=1 ../..
    make

## Building the project (AFL)

    mkdir builds
    mkdir builds/afl
    cd builds/afl
    CC=../../afl-2.49b/afl-clang CXX=../../afl-2.49b/afl-clang++ cmake -DCMAKE_BUILD_TYPE=Debug ../..
    make

    cd ../../afl-2.49b
    ./afl-fuzz -i ../test_data/ -o test -- ../builds/afl/bin/wasm-dump @@

## Resources

http://llvm.org/docs/LibFuzzer.html
http://lcamtuf.coredump.cx/afl/README.txt
http://webassembly.org/docs/binary-encoding
