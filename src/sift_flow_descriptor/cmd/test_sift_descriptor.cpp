/*  ---------------------------------------------------------------------
    Copyright 2017 Fangjun Kuang
    email: csukuangfj at gmail dot com

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a COPYING file of the GNU General Public License
    along with this program. If not, see <http://www.gnu.org/licenses/>
    -----------------------------------------------------------------  */
#include <iostream>

#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>

#include "SiftDescriptor.hpp"

#include "common.hpp"

// visualize the sift flow descriptor of an image via PCA

int main(int argc, char* argv[])
{
   cv::String s = cv::String(KFJ_DATA_PATH) + "/yos/yos1.pgm";

   if (argc == 2)
   {
      s = std::string(argv[1]);
   }

   cv::Mat m;
   m = cv::imread(s, cv::IMREAD_COLOR);

   cv::Mat sift;
   SiftDescriptor::compute_sift_descriptor(m, sift);

   std::cout << "number of channels: " << sift.channels()
             << std::endl;

   cv::String filename = cv::String(KFJ_DATA_PATH) + "/sift_flow_descriptor/pcSIFT.bin";
   cv::Mat img;
   SiftDescriptor::pca_visualisation(sift, img, filename);

   cv::Mat tmp;
   img.convertTo(tmp, CV_8U, 255);
   cv::imwrite("yos-sift.png", tmp);

#if 0
   cv::namedWindow("original image", 0);
   cv::namedWindow("SIFT", 0);

   cv::imshow("original image", m);
   cv::imshow("SIFT", img);

   cv::waitKey(0);
#endif

   return 0;
}
