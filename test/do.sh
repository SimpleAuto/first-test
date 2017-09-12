#!/bin/bash
echo -e "=============="
echo -e "rm old execute file"
echo -e "======"
rm ctest result
echo -e "=============="
echo -e "making"
echo -e "======"
make
echo -e "=============="
echo -e "ready to run result--->test.cpp"
echo -e "======"
./result
echo -e "=============="
echo -e "ready to run ctest--->test.c"
echo -e "======"
./ctest
echo -e "=============="
