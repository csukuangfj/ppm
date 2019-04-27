#!/bin/bash
pushd .

mkdir -p $HOME/kfj-software/opencv-install
mkdir -p /tmp/fangjun

cd /tmp/fangjun
git clone -b 3.4 https://github.com/opencv/opencv.git
git clone -b 3.4 https://github.com/opencv/opencv_contrib.git

mkdir build
cd build

cmake -D CMAKE_BUILD_TYPE=RELEASE \
      -D CMAKE_INSTALL_PREFIX=$HOME/kfj-software/opencv-install \
      -D WITH_TBB=ON \
      -D BUILD_NEW_PYTHON_SUPPORT=OFF \
      -D WITH_V4L=OFF \
      -D INSTALL_C_EXAMPLES=OFF \
      -D INSTALL_PYTHON_EXAMPLES=OFF \
      -D BUILD_EXAMPLES=OFF \
      -D BUILD_DOCS=OFF \
      -D WITH_QT=OFF \
      -D WITH_OPENGL=OFF \
      -D WITH_CUDA=OFF \
      -D WITH_OPENCL=OFF \
      -D WITH_MATLAB=OFF \
      -D WITH_VTK=OFF \
      -DBUILD_opencv_java=OFF \
      -DBUILD_opencv_ml=OFF \
      -DBUILD_opencv_objdetect=OFF \
      -DBUILD_opencv_superres=OFF \
      -DBUILD_opencv_videostab=OFF \
      \
      -DBUILD_opencv_phase_unwrapping=OFF \
      -DBUILD_opencv_plot=OFF \
      -DBUILD_opencv_reg=OFF \
      -DBUILD_opencv_surface_matching=OFF \
      -DBUILD_opencv_xphoto=OFF \
      -DBUILD_opencv_bgsegm=OFF \
      -DBUILD_opencv_face=OFF \
      -DBUILD_opencv_freetype=OFF \
      -DBUILD_opencv_fuzzy=OFF \
      -DBUILD_opencv_img_hash=OFF \
      -DBUILD_opencv_xobjdetect=OFF \
      -DBUILD_opencv_bioinspired=OFF \
      -DBUILD_opencv_dpm=OFF \
      -DBUILD_opencv_line_descriptor=OFF \
      -DBUILD_opencv_saliency=OFF \
      -DBUILD_opencv_text=OFF \
      -DBUILD_opencv_ccalib=OFF \
      -DBUILD_opencv_cvv=OFF \
      -DBUILD_opencv_datasets=OFF \
      -DBUILD_opencv_rgbd=OFF \
      -DBUILD_opencv_structured_light=OFF \
      -DBUILD_opencv_tracking=OFF \
      -DBUILD_opencv_videostab=OFF \
      -DBUILD_opencv_aruco=OFF \
      -DBUILD_TESTS=OFF -DBUILD_PERF_TESTS=OFF \
      -DOPENCV_EXTRA_MODULES_PATH=../opencv_contrib/modules \
      ../opencv

make -s -j4
make install

popd
