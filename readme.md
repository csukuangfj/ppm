## Build Status

 - Travis CI VM: [![Build Status](https://travis-ci.org/csukuangfj/ppm.svg?branch=master)](https://travis-ci.org/csukuangfj/ppm)

## Introduction

This repository contains the source code for my thesis,
which implements and extends [CPM][1] for motion estimation
and [PatchMatch Stereo][2] for stereo matching.

You can obtain a copy of the thesis at the address: https://elib.uni-stuttgart.de/handle/11682/9592

The estimation results
for the standard benchmarks are available
at: [KITTI-2012][8], [KITTI-2015][9],
[MPI Sintel][10] and [Middlebury][11].

## Usage

It has been tested on macOS and Ubuntu.

To compile the code, you need to install [OpenCV][3]
and its contribution module [opencv_contrib][4].
In addition, your compiler has to support C++11.

1. Download the repository.

2. Build the code

```.sh
cd ppm
mkdir build
cd build
cmake ..
make -j4
```

3. It will generate three demo executables
 - `ppm_cmd` for CPM without post processing (motion estimation)
 - `ppm_cmd_with_epic` for CPM with post processing (motion estimation)
 - `ppm_st_cmd` for stereo matching.

Refer to [ppm_commandline.cpp][5],
[ppm_commandline_with_epic_flow.cpp][6]
and [ppm_st_cmd.cpp][7] for how to use it.

[11]: http://vision.middlebury.edu/flow/eval/results-fangjun-kuang/
[10]: http://sintel.is.tue.mpg.de/results
[9]: http://www.cvlibs.net/datasets/kitti/eval_scene_flow_detail.php?benchmark=flow&result=55531e93f1a96d0f7f49f53bd250c9979c592356
[8]: http://www.cvlibs.net/datasets/kitti/eval_stereo_flow_detail.php?benchmark=flow&error=3&eval=all&result=361e835c95c296c7bd9eddbbf270fa132eddc5ec
[7]: /src/ppm_stereo/cmd/ppm_st_cmd.cpp
[6]: /src/ppm_flow/cmd/ppm_commandline_with_epic_flow.cpp
[5]: /src/ppm_flow/cmd/ppm_commandline.cpp
[4]: https://github.com/opencv/opencv_contrib
[3]: https://github.com/opencv/opencv
[2]: https://www.microsoft.com/en-us/research/publication/patchmatch-stereo-stereo-matching-with-slanted-support-windows/
[1]: https://github.com/YinlinHu/CPM

[![Analytics](https://ga-beacon.appspot.com/UA-112812838-1/welcome-page)](https://github.com/csukuangfj/ppm)
