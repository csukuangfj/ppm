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
#ifndef _pm_common_HPP_
#define _pm_common_HPP_

#include <opencv2/core.hpp>
#include "MatchCost.hpp"

bool
is_inside(int x, int w);

bool
improve_cost(const cv::Mat& f_,
             const cv::Mat& g_,
             int x_,
             int y_,
             float& u_old_,
             float& v_old_,
             float& cost_old_,
             float u_new_,
             float v_new_,
             cv::Ptr<MatchCost> cost_func_ptr_,
             int half_patch_size_);

void
compute_cost(
      const cv::Mat& u_,
      const cv::Mat& v_,
      const cv::Mat& image1_,
      const cv::Mat& image2_,
      cv::Mat& cost_,
      const cv::Ptr<MatchCost> cost_func_ptr_,
      const cv::Mat& seeds_,
      int half_path_size_
);
#endif //_pm_common_HPP_
