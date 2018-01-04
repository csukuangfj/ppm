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
#include <opencv2/imgcodecs.hpp>

#include "FlowFormat_KITTI.hpp"

void
FlowFormat_KITTI::read_flow(
     const std::string &filename_,
     cv::Mat &u_,
     cv::Mat &v_,
     cv::OutputArray& valid_
)
{
   cv::Mat img = cv::imread(filename_, cv::IMREAD_UNCHANGED);

   CV_Assert(!img.empty());
   CV_Assert(img.type() == CV_16UC3);

   int ny = img.rows;
   int nx = img.cols;

   u_.create(ny, nx, CV_32FC1);
   v_.create(ny, nx, CV_32FC1);

   cv::Mat valid;
   if (valid_.kind() == cv::_InputArray::NONE)
   {
      valid.create(ny, nx, CV_8UC1);
   }
   else
   {
      valid_.create(ny, nx, CV_8UC1);
      valid = valid_.getMat();
   }

   for (int y = 0; y < ny; y++)
   {
      for (int x = 0; x < nx; x++)
      {
         const cv::Vec3w& pixel = img.at<cv::Vec3w>(y,x);
         const ushort& r = pixel[2]; // u
         const ushort& g = pixel[1]; // v
         const ushort& b = pixel[0]; // 1-valid, 0-invalid

         if (b)
         {
            u_.at<float>(y,x) = ((float) r - (1<<15)) / 64.0f;
            v_.at<float>(y,x) = ((float) g - (1<<15)) / 64.0f;
            valid.at<uchar>(y,x) = 1;
         }
         else
         {
            u_.at<float>(y,x) = 0;  // TODO: assign a value to invalid flow. Currently, it is assigned to 0
            v_.at<float>(y,x) = 0;
            valid.at<uchar>(y,x) = 0;
         }
      }
   }
}

void
FlowFormat_KITTI::write_flow(
     const std::string &filename_,
     const cv::Mat &u_,
     const cv::Mat &v_,
     const cv::Mat& valid_
)
{
   CV_Assert(u_.size == v_.size);
   CV_Assert(u_.type() == v_.type());
   CV_Assert(u_.type() == CV_32FC1);
   CV_Assert(valid_.empty() || valid_.size == u_.size);
   CV_Assert(valid_.empty() || valid_.type() == CV_8UC1);

   std::string filename(filename_);

   int name_length = (int)filename.size();
   CV_Assert(name_length > 4);
   if ( (filename_[name_length-4] != '.') ||
        (filename_[name_length-3] != 'p') ||
        (filename_[name_length-2] != 'n') ||
        (filename_[name_length-1] != 'g'))
   {
      filename += ".png";
   }

   int ny = u_.rows;
   int nx = u_.cols;

   cv::Mat valid = valid_.clone();
   if (valid.empty())
   {
      valid = cv::Mat::ones(u_.size(), CV_8UC1);
   }

   cv::Mat flow;
   flow.create(ny, nx, CV_16UC3);

   for (int y = 0; y < ny; y++)
   {
      for (int x = 0; x < nx; x++)
      {
         float t;
         t = u_.at<float>(y,x);
         t *= 64;
         t += 1<<15;
         t = cv::min(t, (float)((1<<16)-1));
         t = cv::max(t, 0.0f);

         ushort r = (ushort)t;

         t = v_.at<float>(y,x);
         t *= 64;
         t += 1<<15;
         t = cv::min(t, (float)((1<<16)-1));
         t = cv::max(t, 0.0f);

         ushort g = (ushort)t;

         ushort b = valid.at<uchar>(y,x);

         flow.at<cv::Vec3w>(y,x) = cv::Vec3w(b,g,r);
      }
   }

   cv::imwrite(filename, flow);
}
