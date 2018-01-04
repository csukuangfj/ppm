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
#ifndef _StereoFormatPfm_HPP_
#define _StereoFormatPfm_HPP_

#include "StereoFormat.hpp"

class StereoFormatPfm : public StereoFormat
{
public:

   virtual cv::Mat read_disparity_from_file(
         const cv::String& filename_,
         cv::OutputArray& valid_ = cv::noArray(),
         float* options_ = nullptr,
         int options_len_ = 0
   );

   virtual void write_disparity_to_file(
         const cv::String& filename_,
         const cv::Mat& disparity_,
         const float* options_ = nullptr,
         int options_len_ = 0
   );
};

#endif //_StereoFormatPfm_HPP_
