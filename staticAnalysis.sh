#!/usr/bin/env bash

# Run CPPCheck
echo "
**************************************************
Starting CPPCheck
**************************************************"
cppcheck src tests -iexternal/ -I src/ -I tests/ --enable=all --quiet --suppress=missingIncludeSystem

# Run Clang-Tidy using cmake
echo "
**************************************************
Starting Clang-Tidy
**************************************************"
cmake -DBUILD_TESTING=ON -DCODE_COVERAGE=OFF -DSTATIC_ANALYSIS=OFF -DCMAKE_BUILD_TYPE=Debug -DCMAKE_CXX_CLANG_TIDY=clang-tidy-9 .
cmake --build . --clean-first -- -j $(nproc)

# OCLint quality reporting
echo "
**************************************************
Starting OCLint
**************************************************"
cmake -DCMAKE_CXX_FLAGS="-g -O0" -DCMAKE_EXE_LINKER_FLAGS="-g -O0" -DCMAKE_EXPORT_COMPILE_COMMANDS=ON -DSTATIC_ANALYSIS=OFF -UCMAKE_CXX_CLANG_TIDY .
cmake --build . --clean-first -- -j $(nproc)
oclint-json-compilation-database -i src -i tests -e external -- -rc LONG_LINE=120