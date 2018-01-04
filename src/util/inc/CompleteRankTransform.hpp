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
#ifndef _CompleteRankTransform_HPP_
#define _CompleteRankTransform_HPP_

#include <opencv2/core.hpp>

/**
 * Compute the complete rank transform.
 *
 * Reference:
 *  Demetz, Oliver, David Hafner, and Joachim Weickert.
 *  "The Complete Rank Transform: A Tool for Accurate and Morphologically Invariant
 *  Matching of Structures." BMVC. 2013.
 *
 * @param in_image_         [in] depth is CV_8U or CV_32F, number of channels is 1 or 3
 * @param rank_image_       [out] depth is CV_8U, number of channels is wnd_size*wnd_size
 * @param wnd_size_         [in] An odd number less than 16
 * @param color_to_gray_    [in] true to convert the color image to a gray image and compute the complete rank
 *                               over the gray image.
 *                               false to compute the complete rank over each channel and concatenate
 *                               the results at the end.
 */
void
complete_rank_transform(
      const cv::Mat& in_image_,
      cv::Mat& rank_image_,
      int wnd_size_,
      bool color_to_gray_ = true
);

#endif //_CompleteRankTransform_HPP_
