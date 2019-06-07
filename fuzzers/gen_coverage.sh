#!/bin/bash
LLVM_PROFILE_FILE=FuzzLibwasm.profraw fuzzers/FuzzLibwasm -runs=1 corpus/
llvm-profdata-8 merge -sparse FuzzLibwasm.profraw -o FuzzLibwasm.profdata
llvm-cov-8 show fuzzers/FuzzLibwasm -instr-profile=FuzzLibwasm.profdata -use-color -format=html > libwasm_coverage.html
llvm-cov-8 report fuzzers/FuzzLibwasm -instr-profile=FuzzLibwasm.profdata
