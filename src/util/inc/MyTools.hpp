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
#ifndef __MYTOOLS_HPP__
#define __MYTOOLS_HPP__

#include <vector>
#include <opencv2/core.hpp>

/**
 * It contains various utilities.
 */
class MyTools
{
public:
   /**
    * Concatenate the images of a pyramid into a single image.
    *
    * It is useful for displaying purposes.
    *
    * The following shows an example
    *
    * @verbatim
    *  _________________________
    * |                |        |
    * |                |        |
    * |                |        |
    * |                |________|
    * |                |   |
    * |                |___|
    * |________________|
    *
    * @endverbatim
    *
    * @param pyramid [in] Image pyramid. The 0th image is the finest level. All levels should have the same type.
    * @param img     [out] All levels are integrated into this image.
    *
    */
   static void concatenate_images(
        const std::vector<cv::Mat>& pyramid_,
        cv::Mat& img_
   );

   /**
    * Check whether a given file exists or not.
    * @param filename_  [in] The filename to check.
    * @return Return true if the file exists, false otherwise.
    */
   static bool file_exists(const std::string& filename_);

   /**
    * Check whether a given directory exists or not.
    * @param path_  [in] The directory path.
    * @return Return true if the directory exists, false otherwise.
    */
   static bool directory_exists(const std::string& path_);

   /**
    *  Create a directory. It invokes `mkdir -p`.
    *  That is, it will create the intermediate directories if they do not exist.
    *
    *  Note that it works only on Linux and Mac OS X.
    * @param path_ [in] The directory path.
    */
   static void create_directory(const std::string& path_);
};

#endif //__MYTOOLS_HPP__
