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
#include "CpmImplAffineModel.hpp"

/*
[
 x2 - x1      a1  a2       x1       a5
          =            *         +
 y2 - y1      a3  a4       y1       a6
]

 u = x2-x1 = a1*x1 + a2*y1 + a5
 v = y2-y1 = a3*x1 + a4*y1 + a6
*/

CpmImplAffineModel::CpmImplAffineModel()
{
   set_num_properties(6);

   set_min_search_value(1e-3f);
}

void
CpmImplAffineModel::property_random_init(
      cv::Mat &property_,
      const cv::Mat &seeds_,
      float max_property_value_
)
{
   CV_Assert(property_.type() == CV_32FC(6));
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
   }
}

void
CpmImplAffineModel::property_to_uv(
      const cv::Mat &property_,
      const cv::Mat& seeds_,
      cv::Mat &u_,
      cv::Mat &v_
)
{
   CV_Assert(property_.type() == CV_32FC(6));
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

      float u = a1*x + a2*y + a5;
      float v = a3*x + a4*y + a6;
      u_.at<float>(y,x) = u;
      v_.at<float>(y,x) = v;
   }
}

void
CpmImplAffineModel::property_to_uv(
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

   u_ = a1*x_ + a2*y_ + a5;
   v_ = a3*x_ + a4*y_ + a6;
}

void
CpmImplAffineModel::random_search(
      const float *p_old_property_,
      float *p_try_property_,
      float delta_
)
{
   (void)p_old_property_;

   float delta_a1, delta_a2, delta_a3, delta_a4, delta_a5, delta_a6;
   delta_a1 = cv::theRNG().uniform(-delta_, delta_);
   delta_a2 = cv::theRNG().uniform(-delta_, delta_);
   delta_a3 = cv::theRNG().uniform(-delta_, delta_);
   delta_a4 = cv::theRNG().uniform(-delta_, delta_);
   delta_a5 = cv::theRNG().uniform(-delta_, delta_);
   delta_a6 = cv::theRNG().uniform(-delta_, delta_);

   float try_a1, try_a2, try_a3, try_a4, try_a5, try_a6;
#if 0
   try_a1 = p_old_property_[0] + delta_a1;
   try_a2 = p_old_property_[1] + delta_a2;
   try_a3 = p_old_property_[2] + delta_a3;
   try_a4 = p_old_property_[3] + delta_a4;
   try_a5 = p_old_property_[4] + delta_a5;
   try_a6 = p_old_property_[5] + delta_a6;
#else
   try_a1 = delta_a1;
   try_a2 = delta_a2;
   try_a3 = delta_a3;
   try_a4 = delta_a4;
   try_a5 = delta_a5;
   try_a6 = delta_a6;
#endif

   p_try_property_[0] = try_a1;
   p_try_property_[1] = try_a2;
   p_try_property_[2] = try_a3;
   p_try_property_[3] = try_a4;
   p_try_property_[4] = try_a5;
   p_try_property_[5] = try_a6;
}

void
CpmImplAffineModel::init_from_coarser_level(
      const float *p_coarse_level_property_,
      float *p_fine_level_property_,
      float scale_
)
{
   // a1' = a1
   // a2' = a2
   // a3' = a3
   // a4' = a4
   // a5' = a5*scale_
   // a6' = a6*scale_
   p_fine_level_property_[0] = p_coarse_level_property_[0]; // a1
   p_fine_level_property_[1] = p_coarse_level_property_[1]; // a2
   p_fine_level_property_[2] = p_coarse_level_property_[2]; // a3
   p_fine_level_property_[3] = p_coarse_level_property_[3]; // a4
   p_fine_level_property_[4] = p_coarse_level_property_[4] * scale_; // a5
   p_fine_level_property_[5] = p_coarse_level_property_[5] * scale_; // a6
}
