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
#include "PatchMatchStereoImplTranslationalModel.hpp"

PatchMatchStereoImplTranslationalModel::PatchMatchStereoImplTranslationalModel()
{
   set_num_properties(1);

   set_min_search_value(0.1f);
}

void
PatchMatchStereoImplTranslationalModel::property_random_init(
      float *p_property_,
      float value_range_,
      int /*x_*/,
      int /*y_*/
)
{
   p_property_[0] = cv::theRNG().uniform(0.0f, value_range_); // disparity is always positive
}

float
PatchMatchStereoImplTranslationalModel::compute_disparity(
      const float *p_property_,
      int /*x_*/,
      int /*y_*/,
      bool /*is_left_view_*/
)
{
   return p_property_[0];
}

void
PatchMatchStereoImplTranslationalModel::property_view_conversion(
      const float *p_property_in_,
      float *p_property_out_,
      int /*x_*/,
      int /*y_*/,
      float /*z_*/,
      bool /*is_left_view*/
)
{
   p_property_out_[0] = p_property_in_[0]; // left view and right view have the same disparity
}

void
PatchMatchStereoImplTranslationalModel::random_search(
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
#else
   p_try_property_[0] = cv::theRNG().uniform(-delta_, delta_);
#endif
}

void
PatchMatchStereoImplTranslationalModel::copy_properties(
      const float *p_property_in_,
      float *p_property_out_,
      int /*x_*/,
      int /*y_*/,
      float /*z_*/
)
{
   p_property_out_[0] = p_property_in_[0];
}
