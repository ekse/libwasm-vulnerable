# libwasm

libwasm is a purposefully vulnerable library that was built for a fuzzing workshop. You can
find the workshop slides below with the vulnerabilities at the end.

https://docs.google.com/presentation/d/1Qol9ozp2-YL3OQ5YZap9jA15Q3qYUTjg3_vJdrmZcfQ/

On Ubuntu:
    apt install clang-8 clang-tools-8 lldb-8 libfuzzer-8-dev cmake gdb

## Building the project (libFuzzer)

Build libwasm with libFuzzer.

    mkdir -p builds/libFuzzer
    cd builds/libFuzzer
    CC=clang-8 CXX=clang++-8 cmake -DWASM_FUZZING=ON ../..
    make

Start the fuzzer.

    mkdir corpus
    ./fuzzers/libFuzzer corpus/

Generate code coverage data.

    ../../fuzzers/gen_coverage.sh

## Building the project (AFL)

As root, configure your system for fuzzing.

    echo core > /proc/sys/kernel/core_pattern

Build afl.

    cd afl-2.49b
    make

Build libwasm with afl support.

    mkdir -p builds/afl
    cd builds/afl
    CC=../../afl-2.49b/afl-gcc CXX=../../afl-2.49b/afl-g++ cmake ../..
    make

Start fuzzing.

    cd ../../afl-2.49b
    ./afl-fuzz -i ../test_data/ -o afl_results -- ../builds/afl/bin/wasm-dump @@

## Resources

* http://llvm.org/docs/LibFuzzer.html
* http://lcamtuf.coredump.cx/afl/README.txt
* http://webassembly.org/docs/binary-encoding
