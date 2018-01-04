#!/bin/bash

mkdir build
cd build

cmake -Dkfj_enable_test=ON ..
make -s -j4

./gtest

./ppm_cmd

./ppm_cmd_with_epic
./ppm_cmd_with_epic 1
./ppm_cmd_with_epic 2

./ppm_st_cmd