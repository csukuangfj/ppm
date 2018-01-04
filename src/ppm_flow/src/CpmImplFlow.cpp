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
#include "CpmImplFlow.hpp"

/*
column 0: u
column 1: v
 */
CpmImplFlow::CpmImplFlow()
{
   set_num_properties(2);

   set_min_search_value(0.1f);
}

void
CpmImplFlow::property_random_init(
      cv::Mat &property_,
      const cv::Mat &seeds_,
      float max_property_value_
)
{
   CV_Assert(property_.type() == CV_32FC2);
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
   }
}

void
CpmImplFlow::property_to_uv(
      const cv::Mat &property_,
      const cv::Mat& /*seeds_*/,
      cv::Mat &u_,
      cv::Mat &v_
)
{
   CV_Assert(property_.type() == CV_32FC2);

   cv::Mat mv[2];
   cv::split(property_, mv);
   u_ = mv[0];
   v_ = mv[1];
}

void
CpmImplFlow::property_to_uv(
      const float *p_property_,
      float &u_,
      float &v_,
      int /*x_*/,
      int /*y_*/
)
{
   u_ = p_property_[0];
   v_ = p_property_[1];
}

void
CpmImplFlow::random_search(
      const float *p_old_property_,
      float *p_try_property_,
      float delta_
)
{
   (void)p_old_property_;

   float delta_u = cv::theRNG().uniform(-delta_, delta_);
   float delta_v;
   delta_v = cv::theRNG().uniform(-delta_, delta_);
#if 0
   p_try_property_[0] =  delta_u;
   p_try_property_[1] =  delta_v;
#else
   p_try_property_[0] = p_old_property_[0] + delta_u;
   p_try_property_[1] = p_old_property_[1] + delta_v;
#endif
}

void
CpmImplFlow::init_from_coarser_level(
      const float *p_coarse_level_property_,
      float *p_fine_level_property_,
      float scale_
)
{
   p_fine_level_property_[0] = p_coarse_level_property_[0]*scale_;
   p_fine_level_property_[1] = p_coarse_level_property_[1]*scale_;
}
