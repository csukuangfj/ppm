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
#include "HammingCost.hpp"

double
HammingCost::compute_cost(
      const cv::Mat& image1_,
      const cv::Mat& image2_,
      const cv::Point& pixel1_,
      const cv::Point& pixel2_,
      int half_patch_size_
)
{
   CV_Assert(image1_.depth() == CV_8U);
   CV_Assert(image2_.depth() == CV_8U);

   cv::Rect r1(cv::Point(0,0), image1_.size());
   cv::Rect r2(cv::Point(0,0), image2_.size());

   CV_Assert(r1.contains(pixel1_));
   CV_Assert(r2.contains(pixel2_));

   // assume that the patch size is square and has an odd side length
   cv::Rect s1(pixel1_.x - half_patch_size_, pixel1_.y - half_patch_size_, 2*half_patch_size_+1, 2*half_patch_size_+1);
   cv::Rect s2(pixel2_.x - half_patch_size_, pixel2_.y - half_patch_size_, 2*half_patch_size_+1, 2*half_patch_size_+1);

   s1 &= r1;
   s2 &= r2;

   cv::Mat m1 = image1_(s1);
   cv::Mat m2 = image2_(s2);

   cv::Rect s11(cv::Point(0,0), m1.size());
   cv::Rect s22(cv::Point(0,0), m2.size());

   cv::Rect s3 = s11 & s22;

   cv::Mat m11 = m1(s3);
   cv::Mat m22 = m2(s3);

   double res = cv::norm(m11.reshape(1), m22.reshape(1), cv::NORM_HAMMING);
   res /= m11.total() * (size_t)m11.channels();

   return res;
}

double
HammingCost::compute_cost(
      const cv::Mat &image1_,
      const cv::Mat &image2_)
{
   CV_Assert(image1_.size() == image2_.size());
   CV_Assert(image1_.type() == image2_.type());

   double res = cv::norm(image1_, image2_, cv::NORM_HAMMING);
   res /= image1_.total() * (size_t)image2_.channels();
   return res;
}
