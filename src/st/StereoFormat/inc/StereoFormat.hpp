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
#ifndef _StereoFormat_HPP_
#define _StereoFormat_HPP_

#include <opencv2/core.hpp>

class StereoFormat
{
public:
   enum class Format
   {
      E_PFM = 0,  //!< pfm format
   };

   /**
    * Read disparity from file.
    *
    * @param filename_  [in] Filename, from which the disparity is read
    * @param options_
    * @param valid_     [out]  CV_8UC1, 1-valid, 0-invalid
    * @param options_len_ [in] number of elements in options_
    * @return   CV_32FC1 or CV_32FC3
    */
   virtual cv::Mat read_disparity_from_file(
         const cv::String& filename_,
         cv::OutputArray& valid_ = cv::noArray(),
         float* options_ = nullptr,
         int options_len_ = 0
   ) = 0;

   /**
    * Write disparity to file.
    *
    * @param filename_  [in] Filename, to which the disparity is saved
    * @param disparity_ [in] CV_32FC1 or CV_32FC3
    * @param options_
    * @param options_len_ [in] number of elements in options_
    */
   virtual void write_disparity_to_file(
         const cv::String& filename_,
         const cv::Mat& disparity_,
         const float* options_ = nullptr,
         int options_len_ = 0
   ) = 0;

   static cv::Ptr<StereoFormat> create(Format format_);
   static cv::Ptr<StereoFormat> create(const cv::String& filename_);
};

#endif //_StereoFormat_HPP_
