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
#ifndef _PatchMatchStereoImplSlantedPlane_HPP_
#define _PatchMatchStereoImplSlantedPlane_HPP_

#include "PatchMatchStereoImpl.hpp"

class PatchMatchStereoImplSlantedPlane : public PatchMatchStereoImpl
{
public:
   PatchMatchStereoImplSlantedPlane();

   virtual void property_random_init(
         float* p_property_,
         float value_range_,
         int x_,
         int y_
   ) override;

   virtual float compute_disparity(
         const float* p_property_,
         int x_,
         int y_,
         bool is_left_view_
   ) override;

   virtual void property_view_conversion(
         const float* p_property_in_,
         float* p_property_out_,
         int x_,
         int y_,
         float z_,
         bool is_left_view
   ) override;

   virtual void random_search(
         const float *p_old_property_,
         float *p_try_property_,
         float delta_,
         int x_,
         int y_,
         int num_iter_,
         bool is_left_view_
   ) override;

   virtual void copy_properties(
         const float* p_property_in_,
         float* p_property_out_,
         int x_,
         int y_,
         float z_
   ) override;
};

#endif //_PatchMatchStereoImplSlantedPlane_HPP_
