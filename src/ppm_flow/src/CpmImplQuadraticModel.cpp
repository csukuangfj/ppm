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
#include "CpmImplQuadraticModel.hpp"

/*
Refereneces:
Rigid body model
 1. http://citeseerx.ist.psu.edu/viewdoc/download?doi=10.1.1.69.3970&rep=rep1&type=pdf
Passive navigation
AR Bruss, BKP Horn - Computer Vision, Graphics, and Image Processing, 1983 - Elsevier

It consists of 6 parameters

Refer to the book
 Digital Video Processing 1st Edition by A. Murat Tekalp (Author)
which can be downloaded from
 https://www.researchgate.net/profile/A_Tekalp/publication/200132428_Digital_Video_Processing/links/0c96051c469546bb98000000/Digital-Video-Processing.pdf

 Page 182 and 183 present the model for affine flow and quadratic flow

*/

/*
  u = a1 + a2*x + a3*y + a7*x*x + a8*x*y
  v = a4 + a5*x + a6*y + a7*x*y + a8*y*y
*/

CpmImplQuadraticModel::CpmImplQuadraticModel()
{
   set_num_properties(8);

   set_min_search_value(1e-5f);
}

void
CpmImplQuadraticModel::property_random_init(
      cv::Mat &property_,
      const cv::Mat &seeds_,
      float max_property_value_)
{
   CV_Assert(property_.type() == CV_32FC(8));
   CV_Assert(seeds_.type() == CV_32SC1);
   CV_Assert(seeds_.cols == 2);

   property_ = 0;

   int num_seeds = seeds_.rows;

   for (int i = 0; i < num_seeds; i++)
   {
      int y = seeds_.at<int>(i, 1);
      int x = seeds_.at<int>(i, 0);
      float* p_property = property_.ptr<float>(y,x);

      p_property[0] = cv::theRNG().uniform(-max_property_value_, max_property_value_);
      p_property[1] = cv::theRNG().uniform(-max_property_value_, max_property_value_);
      p_property[2] = cv::theRNG().uniform(-max_property_value_, max_property_value_);
      p_property[3] = cv::theRNG().uniform(-max_property_value_, max_property_value_);
      p_property[4] = cv::theRNG().uniform(-max_property_value_, max_property_value_);
      p_property[5] = cv::theRNG().uniform(-max_property_value_, max_property_value_);
      p_property[6] = cv::theRNG().uniform(-max_property_value_, max_property_value_);
      p_property[7] = cv::theRNG().uniform(-max_property_value_, max_property_value_);
   }
}

void
CpmImplQuadraticModel::property_to_uv(
      const cv::Mat &property_,
      const cv::Mat& seeds_,
      cv::Mat &u_,
      cv::Mat &v_
)
{
   CV_Assert(property_.type() == CV_32FC(8));
   u_ = cv::Mat::zeros(property_.size(), CV_32FC1);
   v_ = cv::Mat::zeros(property_.size(), CV_32FC1);

   int num_seeds = seeds_.rows;
   for (int i = 0; i < num_seeds; i++)
   {
      int y = seeds_.at<int>(i, 1);
      int x = seeds_.at<int>(i, 0);

      const float* p_property = property_.ptr<float>(y,x);
      float a1 = p_property[0];
      float a2 = p_property[1];
      float a3 = p_property[2];
      float a4 = p_property[3];
      float a5 = p_property[4];
      float a6 = p_property[5];
      float a7 = p_property[6];
      float a8 = p_property[7];

      float xx = x*x;
      float xy = x*y;
      float yy = y*y;

      float u = a1 + a2*x + a3*y + a7*xx + a8*xy;
      float v = a4 + a5*x + a6*y + a7*xy + a8*yy;

      u_.at<float>(y,x) = u;
      v_.at<float>(y,x) = v;
   }
}

void
CpmImplQuadraticModel::property_to_uv(
      const float *p_property_,
      float &u_,
      float &v_,
      int x_,
      int y_
)
{
   float a1 = p_property_[0];
   float a2 = p_property_[1];
   float a3 = p_property_[2];
   float a4 = p_property_[3];
   float a5 = p_property_[4];
   float a6 = p_property_[5];
   float a7 = p_property_[6];
   float a8 = p_property_[7];

   float xx = x_*x_;
   float xy = x_*y_;
   float yy = y_*y_;

   u_ = a1 + a2*x_ + a3*y_ + a7*xx + a8*xy;
   v_ = a4 + a5*x_ + a6*y_ + a7*xy + a8*yy;
}

void
CpmImplQuadraticModel::random_search(
      const float *p_old_property_,
      float *p_try_property_,
      float delta_
)
{
   (void)p_old_property_;
   float delta_a1, delta_a2, delta_a3, delta_a4;
   float delta_a5, delta_a6, delta_a7, delta_a8;

   delta_a1 = cv::theRNG().uniform(-delta_, delta_);
   delta_a2 = cv::theRNG().uniform(-delta_, delta_);
   delta_a3 = cv::theRNG().uniform(-delta_, delta_);
   delta_a4 = cv::theRNG().uniform(-delta_, delta_);
   delta_a5 = cv::theRNG().uniform(-delta_, delta_);
   delta_a6 = cv::theRNG().uniform(-delta_, delta_);
   delta_a7 = cv::theRNG().uniform(-delta_, delta_);
   delta_a8 = cv::theRNG().uniform(-delta_, delta_);

   float try_a1, try_a2, try_a3, try_a4;
   float try_a5, try_a6, try_a7, try_a8;
#if 0
   try_a1 = p_old_property_[0] + delta_a1;
   try_a2 = p_old_property_[1] + delta_a2;
   try_a3 = p_old_property_[2] + delta_a3;
   try_a4 = p_old_property_[3] + delta_a4;
   try_a5 = p_old_property_[4] + delta_a5;
   try_a6 = p_old_property_[5] + delta_a6;
   try_a7 = p_old_property_[6] + delta_a7;
   try_a8 = p_old_property_[7] + delta_a8;
#else
   try_a1 = delta_a1;
   try_a2 = delta_a2;
   try_a3 = delta_a3;
   try_a4 = delta_a4;
   try_a5 = delta_a5;
   try_a6 = delta_a6;
   try_a7 = delta_a7;
   try_a8 = delta_a8;
#endif
   p_try_property_[0] = try_a1;
   p_try_property_[1] = try_a2;
   p_try_property_[2] = try_a3;
   p_try_property_[3] = try_a4;
   p_try_property_[4] = try_a5;
   p_try_property_[5] = try_a6;
   p_try_property_[6] = try_a7;
   p_try_property_[7] = try_a8;
}

void
CpmImplQuadraticModel::init_from_coarser_level(
      const float *p_coarse_level_property_,
      float *p_fine_level_property_,
      float scale_
)
{
   CV_Assert(scale_ > 1e-3f);

   // a1' = a1 * scale_
   // a2' = a2
   // a3' = a3
   // a4' = a4 * scale_
   // a5' = a5
   // a6' = a6
   // a7' = a7 / scale_
   // a8' = a8 / scale
   p_fine_level_property_[0] = p_coarse_level_property_[0]*scale_;   // a1
   p_fine_level_property_[1] = p_coarse_level_property_[1];          // a2
   p_fine_level_property_[2] = p_coarse_level_property_[2];          // a3
   p_fine_level_property_[3] = p_coarse_level_property_[3]*scale_;   // a4
   p_fine_level_property_[4] = p_coarse_level_property_[4];          // a5
   p_fine_level_property_[5] = p_coarse_level_property_[5];          // a6
   p_fine_level_property_[6] = p_coarse_level_property_[6]/scale_;   // a7
   p_fine_level_property_[7] = p_coarse_level_property_[7]/scale_;   // a8
}
