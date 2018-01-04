#!/bin/bash

# install gtest

gcc --version
g++ --version
cmake --version

pushd .

mkdir -p $HOME/kfj-software/googletest
cd /tmp
git clone https://github.com/google/googletest.git
mkdir build
cd build
cmake -DCMAKE_INSTALL_PREFIX=$HOME/kfj-software/googletest \
      -Dgtest_build_samples=OFF \
      -Dgtest_build_tests=OFF \
      -Dgmock_build_tests=OFF \
      -DCMAKE_CXX_FLAGS=-std=c++11 \
      ../googletest

make -j8
make install

popd
