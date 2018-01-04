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
#include "PatchMatchStereoImplProjectivePlanar.hpp"

/*

 [
  h1  h2  h3
  h4  h5  h6
 ]
 */

PatchMatchStereoImplProjectivePlanar::PatchMatchStereoImplProjectivePlanar()
{
   set_num_properties(6);

   set_min_search_value(1e-3);
}

void
PatchMatchStereoImplProjectivePlanar::property_random_init(
      float *p_property_,
      float value_range_,
      int /*x_*/,
      int /*y_*/)
{
   p_property_[0] = cv::theRNG().uniform(-value_range_, value_range_); // h1
   p_property_[1] = cv::theRNG().uniform(-value_range_, value_range_); // h2
   p_property_[2] = cv::theRNG().uniform(-value_range_, value_range_); // h3

   p_property_[3] = cv::theRNG().uniform(-value_range_, value_range_); // h4
   p_property_[4] = cv::theRNG().uniform(-value_range_, value_range_); // h5
   p_property_[5] = cv::theRNG().uniform(-value_range_, value_range_); // h6
}

float
PatchMatchStereoImplProjectivePlanar::compute_disparity(
      const float *p_property_,
      int x_,
      int y_,
      bool is_left_view_)
{
   float h1 = p_property_[0];
   float h2 = p_property_[1];
   float h3 = p_property_[2];

   float h4 = p_property_[3];
   float h5 = p_property_[4];
   float h6 = p_property_[5];

   int x1 = x_;
   int y1 = y_;
   int y2 = y_;

   float numerator   = h1 * x1 + h2 * y1 + h3;
   float denominator = h4 * x1 + h5 * y1 + h6;
   if (fabs(denominator) < 1e-5f)
   {
      if (denominator > 0)
         denominator = 1e-5f;
      else
         denominator = -1e-5f;
   }

   float x2;
   x2 = numerator * y2 / denominator;

   float d;
   d = is_left_view_ ? (x1 - x2) : (x2 - x1);
   return d;
}

void
PatchMatchStereoImplProjectivePlanar::property_view_conversion(
      const float *p_property_in_,
      float *p_property_out_,
      int x_,
      int /*y_*/,
      float z_,
      bool is_left_view
)
{
   p_property_out_[0] = -p_property_in_[4]; // h1' = -h5
   p_property_out_[1] = p_property_in_[1]; // h2' = h2
   p_property_out_[2] = p_property_in_[2]; // h3' = h3

   p_property_out_[3] = p_property_in_[3]; // h4' = h4
   p_property_out_[4] = -p_property_in_[0]; // h5' = -h1

   int sign = 1;
   if (!is_left_view) sign = -1;

   // todo: check the following expression !!!
   p_property_out_[5] = p_property_in_[5] * (x_ - sign*z_) / (x_ + 1e-5f); // h6' = h6 * x2 / x1
}

void
PatchMatchStereoImplProjectivePlanar::random_search(
      const float *p_old_property_,
      float *p_try_property_,
      float delta_,
      int /*x_*/,
      int /*y_*/,
      int /*num_iter_*/,
      bool /*is_left_view_*/
)
{
#if 1
   p_try_property_[0] = p_old_property_[0] + cv::theRNG().uniform(-delta_, delta_);
   p_try_property_[1] = p_old_property_[1] + cv::theRNG().uniform(-delta_, delta_);
   p_try_property_[2] = p_old_property_[2] + cv::theRNG().uniform(-delta_, delta_);

   p_try_property_[3] = p_old_property_[3] + cv::theRNG().uniform(-delta_, delta_);
   p_try_property_[4] = p_old_property_[4] + cv::theRNG().uniform(-delta_, delta_);
   p_try_property_[5] = p_old_property_[5] + cv::theRNG().uniform(-delta_, delta_);
#else
   p_try_property_[0] = cv::theRNG().uniform(-delta_, delta_);
   p_try_property_[1] = cv::theRNG().uniform(-delta_, delta_);
   p_try_property_[2] = cv::theRNG().uniform(-delta_, delta_);

   p_try_property_[3] = cv::theRNG().uniform(-delta_, delta_);
   p_try_property_[4] = cv::theRNG().uniform(-delta_, delta_);
   p_try_property_[5] = cv::theRNG().uniform(-delta_, delta_);
#endif
}

void
PatchMatchStereoImplProjectivePlanar::copy_properties(
      const float *p_property_in_,
      float *p_property_out_,
      int /*x_*/,
      int /*y_*/,
      float /*z_*/
)
{
   p_property_out_[0] = p_property_in_[0];
   p_property_out_[1] = p_property_in_[1];
   p_property_out_[2] = p_property_in_[2];

   p_property_out_[3] = p_property_in_[3];
   p_property_out_[4] = p_property_in_[4];
   p_property_out_[5] = p_property_in_[5];
}
