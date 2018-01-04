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
#ifndef _CensusTransform_HPP_
#define _CensusTransform_HPP_

/**
 * Census transform.
 *
 * The result of 8 neighbors is packed into a byte. The order of neighbors is
 *
 * 7 6 5
 * 4 x 3
 * 2 1 0
 *
 * For example, if the input image is
 *
 * 10 2 0
 * 5  2 1
 * 1  0 3
 *
 * Then after census transform (for the center pixel), it gets
 *
 * 0 0 1
 * 0   1
 * 1 1 0
 *
 * The resulting byte is 0b00101110.
 *
 * It uses reflexive boundary (gfedcb|abcdefgh|gfedcba).
 *
 * @param in_image_     [in] Input image of type CV_8UC1
 * @param census_image_ [in] Output census image of type CV_8UC1 if wnd_size_ is 3, otherwise CV_8UC3.
 *                           It has the same size with the input image.
 * @param wnd_size_     [in] It must be 3 or 5
 * @param color_to_gray_ [in] false to treat RGB channels separately and concatenate their results.
 *                            true to convert the color image to a gray image and compute the census
 *                            on the gray image
 */
void census_transform(
      const cv::Mat& in_image_,
      cv::Mat& census_image_,
      int wnd_size_,
      bool color_to_gray_ = true
);

#endif //_CensusTransform_HPP_
