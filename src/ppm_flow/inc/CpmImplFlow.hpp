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
#ifndef _CpmImplFlow_HPP_
#define _CpmImplFlow_HPP_

#include "CpmImpl.hpp"

/**
 * Translational model in the thesis.
 */
class CpmImplFlow : public CpmImpl
{
public:
   CpmImplFlow();

   virtual void property_random_init(
         cv::Mat& property_,
         const cv::Mat& seeds_,
         float max_property_value_
   ) override;

   virtual void property_to_uv(
         const cv::Mat& property_,
         const cv::Mat& seeds_,
         cv::Mat& u_,
         cv::Mat& v_
   ) override;

   virtual void property_to_uv(
         const float* p_property_,
         float& u_,
         float& v_,
         int x_,
         int y_
   ) override;

   virtual void random_search(
         const float* p_old_property_,
         float* p_try_property_,
         float delta_
   ) override;

   virtual void init_from_coarser_level(
         const float* p_coarse_level_property_,
         float* p_fine_level_property_,
         float scale_
   ) override;
};

#endif //_CpmImplFlow_HPP_
