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
#include "CpmImplProjectivePlanar.hpp"
/*
It needs a higher number of pyramid levels, say 10 with ratio 0.8, grid step 2
 */

static const float g_min_float = 1e-5f; // avoid dividing by zero

CpmImplProjectivePlanar::CpmImplProjectivePlanar()
{
   set_num_properties(9);

   set_min_search_value(1e-6f);
}

void
CpmImplProjectivePlanar::property_random_init(
      cv::Mat &property_,
      const cv::Mat &seeds_,
      float max_property_value_
)
{
   CV_Assert(property_.type() == CV_32FC(9));
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
      p_property[8] = cv::theRNG().uniform(-max_property_value_, max_property_value_);
   }
}

void
CpmImplProjectivePlanar::property_to_uv(
      const cv::Mat &property_,
      const cv::Mat& seeds_,
      cv::Mat &u_,
      cv::Mat &v_
)
{
   CV_Assert(property_.type() == CV_32FC(9));
   u_ = cv::Mat::zeros(property_.size(), CV_32FC1);
   v_ = cv::Mat::zeros(property_.size(), CV_32FC1);

   int num_seeds = seeds_.rows;
   for (int k = 0; k < num_seeds; k++)
   {
      int y = seeds_.at<int>(k, 1);
      int x = seeds_.at<int>(k, 0);
/*
[
 x1       h1  h2  h3      x
 y1  =    h4  h5  h6   *  y
 z1       h7  h8  h9      1
]

         x1     h1*x + h2*y + h3
 x'  = ----- = -----------------
         z1     h7*x + h8*y + h9

         y1     h4*x + h5*y + h6
 y'  = ----- = -----------------
         z1     h7*x + h8*y + h9

  u = x' - x
  v = y' - y

 */
      const float* p_property = property_.ptr<float>(y,x);
      float h1 = p_property[0];
      float h2 = p_property[1];
      float h3 = p_property[2];
      float h4 = p_property[3];
      float h5 = p_property[4];
      float h6 = p_property[5];
      float h7 = p_property[6];
      float h8 = p_property[7];
      float h9 = p_property[8];

      float x1 = h1*x + h2*y + h3;
      float y1 = h4*x + h5*y + h6;
      float z1 = h7*x + h8*y + h9;

      if (cv::abs(z1) < g_min_float)
      {
         if (z1 < 0) z1 = -g_min_float;
         else z1 = +g_min_float;
      }

      float u = x1/z1 - x;
      float v = y1/z1 - y;

      u_.at<float>(y,x) = u;
      v_.at<float>(y,x) = v;
   }
}

void
CpmImplProjectivePlanar::property_to_uv(
      const float *p_property_,
      float &u_,
      float &v_,
      int x_,
      int y_
)
{
   float h1 = p_property_[0];
   float h2 = p_property_[1];
   float h3 = p_property_[2];
   float h4 = p_property_[3];
   float h5 = p_property_[4];
   float h6 = p_property_[5];
   float h7 = p_property_[6];
   float h8 = p_property_[7];
   float h9 = p_property_[8];

   float x1 = h1*x_ + h2*y_ + h3;
   float y1 = h4*x_ + h5*y_ + h6;
   float z1 = h7*x_ + h8*y_ + h9;

   if (cv::abs(z1) < g_min_float)
   {
      if (z1 < 0) z1 = -g_min_float;
      else z1 = +g_min_float;
   }

   u_ = x1/z1 - x_;
   v_ = y1/z1 - y_;
}

void
CpmImplProjectivePlanar::random_search(
      const float *p_old_property_,
      float *p_try_property_,
      float delta_
)
{
   (void)p_old_property_;
   float delta_h1, delta_h2, delta_h3, delta_h4, delta_h5, delta_h6, delta_h7, delta_h8, delta_h9;

   delta_h1 = cv::theRNG().uniform(-delta_, delta_);
   delta_h2 = cv::theRNG().uniform(-delta_, delta_);
   delta_h3 = cv::theRNG().uniform(-delta_, delta_);
   delta_h4 = cv::theRNG().uniform(-delta_, delta_);
   delta_h5 = cv::theRNG().uniform(-delta_, delta_);
   delta_h6 = cv::theRNG().uniform(-delta_, delta_);
   delta_h7 = cv::theRNG().uniform(-delta_, delta_);
   delta_h8 = cv::theRNG().uniform(-delta_, delta_);
   delta_h9 = cv::theRNG().uniform(-delta_, delta_);

   float try_h1, try_h2, try_h3, try_h4, try_h5, try_h6, try_h7, try_h8, try_h9;

#if 1 // for projective, it is better to use this one
   try_h1 = p_old_property_[0] + delta_h1;
   try_h2 = p_old_property_[1] + delta_h2;
   try_h3 = p_old_property_[2] + delta_h3;
   try_h4 = p_old_property_[3] + delta_h4;
   try_h5 = p_old_property_[4] + delta_h5;
   try_h6 = p_old_property_[5] + delta_h6;
   try_h7 = p_old_property_[6] + delta_h7;
   try_h8 = p_old_property_[7] + delta_h8;
   try_h9 = p_old_property_[8] + delta_h9;
#else
   try_h1 = delta_h1;
   try_h2 = delta_h2;
   try_h3 = delta_h3;
   try_h4 = delta_h4;
   try_h5 = delta_h5;
   try_h6 = delta_h6;
   try_h7 = delta_h7;
   try_h8 = delta_h8;
   try_h9 = delta_h9;
#endif

   p_try_property_[0] = try_h1;
   p_try_property_[1] = try_h2;
   p_try_property_[2] = try_h3;
   p_try_property_[3] = try_h4;
   p_try_property_[4] = try_h5;
   p_try_property_[5] = try_h6;
   p_try_property_[6] = try_h7;
   p_try_property_[7] = try_h8;
   p_try_property_[8] = try_h9;
}

void
CpmImplProjectivePlanar::init_from_coarser_level(
      const float *p_coarse_level_property_,
      float *p_fine_level_property_,
      float scale_
)
{
   // h1' = h1
   // h2' = h2
   // h3' = h3 * scale_
   // h4' = h4
   // h5' = h5
   // h6' = h6 * scale_
   // h7' = h7/scale_
   // h8' = h8/scale_
   // h9' = h9
   p_fine_level_property_[0] = p_coarse_level_property_[0];        // h1
   p_fine_level_property_[1] = p_coarse_level_property_[1];        // h2
   p_fine_level_property_[2] = p_coarse_level_property_[2]*scale_; // h3
   p_fine_level_property_[3] = p_coarse_level_property_[3];        // h4
   p_fine_level_property_[4] = p_coarse_level_property_[4];        // h5
   p_fine_level_property_[5] = p_coarse_level_property_[5]*scale_; // h6
   p_fine_level_property_[6] = p_coarse_level_property_[6]/scale_; // h7
   p_fine_level_property_[7] = p_coarse_level_property_[7]/scale_; // h8
   p_fine_level_property_[8] = p_coarse_level_property_[8]; // h9
}
