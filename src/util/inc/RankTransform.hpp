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
#ifndef _RankTransform_HPP_
#define _RankTransform_HPP_

/**
 * Rank transform.
 *
 * @param in_image_      [in] Input image with depth CV_8U or CV_32F, 1-channel or 3-channel
 * @param rank_image_    [out] Output image of type CV_8UC1 and has same size with the input image
 * @param wnd_size_      [in] Neighborhood size. It must be an odd number and be less than 16.
 * @param color_to_gray_ [in] false to treat RGB channels separately and concatenate their result.
 *                            true to convert the color image to gray image and compute the rank
 *                            on the gray image
 */
void rank_transform(
      const cv::Mat& in_image_,
      cv::Mat& rank_image_,
      int wnd_size_,
      bool color_to_gray_ = true
);

#endif //_RankTransform_HPP_
