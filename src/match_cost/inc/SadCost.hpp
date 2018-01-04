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
#ifndef _SADCOST_HPP_
#define _SADCOST_HPP_

#include "MatchCost.hpp"

/**
 * Average sum of absolute distances.
 *
 *  L1-norm
 */
class SadCost : public MatchCost
{
public:
   virtual double compute_cost(
         const cv::Mat& image1_,
         const cv::Mat& image2_,
         const cv::Point& pixel1_,
         const cv::Point& pixel2_,
         int half_patch_size_
   ) override;

   virtual double compute_cost(
         const cv::Mat& image1_,
         const cv::Mat& image2_
   ) override;

   virtual cv::String get_name() const override
   {return "SAD";};
};

#endif //_SADCOST_HPP_
