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
#ifndef __SIFTDESCRIPTOR_HPP__
#define __SIFTDESCRIPTOR_HPP__

#include <opencv2/core.hpp>

class SiftDescriptor
{
public:
   /**
    * Compute SIFT flow descriptor for an image.
    *
    * The default parameters are good enough.
    *
    * @param image_                [in]   Any image format supported by OpenCV
    * @param descriptor_           [out]  CV_8UC(128)
    * @param cell_size_            [in]   half cell size
    * @param step_size_            [in]
    * @param is_boundary_included_ [in]
    * @param num_bins_             [in]   number of bins in a cell
    */
   static void compute_sift_descriptor(
         const cv::Mat& image_,
         cv::Mat& descriptor_,
         int cell_size_ = 2,
         int step_size_ = 1,
         bool is_boundary_included_ = true,
         int num_bins_ = 8
   );

   /**
    * Visualize the sift flow descriptor.
    *
    * @param sift_image_  [in] CV_8UC(128), SIFT descriptor image
    * @param out_img_     [out] CV_32FC3, BGR image, pixel value is in the range [0,1]
    * @param filename_    [in] File name contains the basis vectors of the sift descriptor
    */
   static void pca_visualisation(
         const cv::Mat& sift_image_,
         cv::Mat& out_img_,
         const std::string& filename_
   );
};

#endif //__SIFTDESCRIPTOR_HPP__
