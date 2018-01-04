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
#ifndef _MY_IMAGEPROCESSING_HPP
#define _MY_IMAGEPROCESSING_HPP

#include <opencv2/core.hpp>

/**
 * My image processing class.
 */
class MyImageProcessing
{
public:
   enum InterpolationMode
   {
      E_INTERPOLATION_NN = 0,       //!< nearest neighbor interpolation
      E_INTERPOLATION_BILINEAR = 2, //!< bilinear interpolation
      E_INTERPOLATION_BIBUIC = 1,   //!< bicubic interpolation
   };

public:
   /**
    * Compute gradient in x direction using Sobel operator.
    *      [-1 0 1]
    *      [-2 0 2] x 1/8
    *      [-1 0 1]
    * @param image_  [in] Input image.
    * @param grad_   [out] Output gradient, depth: CV_32F,
    *                      same number of channels with image_.
    */
   static void gradient_x(const cv::Mat& image_, cv::Mat& grad_);

   /**
    * Compute gradient in y direction using Sobel operator
    *      [-1 -2 -1]
    *      [ 0  0  0] x 1/8
    *      [ 1  2  1]
    * @param image_  [in] Input image.
    * @param grad_   [out] Output gradient, depth: CV_32F,
    *                      same number of channels with image_.
    */
   static void gradient_y(const cv::Mat& image_, cv::Mat& grad_);

   /**
    *  Filter the image with specified Gaussian sigma.
    *
    *  x and y direction have the same sigma.
    *
    *  The kernel has a square shape. The length of one side is
    *  @code
    *  cvRound(precision_*sigma_*2 + 1) | 1;
    *  @endcode
    *
    * @param image      [in] Input image.
    * @param out_       [out] filtered image, depth: CV_32F.
    * @param sigma_     [in] sigma in x and y direction.
    * @param precision_ [in] truncate at 5*sigma_.
    */
   static void gaussian_filtering(
         const cv::Mat& image_,
         cv::Mat& out_,
         float sigma_,
         float precision_ = 5
   );

   /**
    * Get image pyramid by specifying the ratio.
    *
    * @param raw_image_     [in]  The raw image.
    * @param pyramids_      [out] Contains the pyramid. The 0th element is the raw image
    *                             and the last element is the coarsest image.
    * @param ratio_         [in]  The ratio between each level. It should be in the range [0.4, 0.98].
    * @param minimum_width_ [in]  Minimum width of the coarsest level.
    *
    * @return Number of levels.
    */
   static int get_image_pyramid_by_ratio(
         const cv::Mat& raw_image_,
         std::vector<cv::Mat>& pyramids_,
         float& ratio_,
         int minimum_width_ = 30
   );

   /**
    * Get image pyramid by specifying the number of levels.
    *
    * @param raw_image_     [in]  The raw image.
    * @param pyramids_      [out] Contains the pyramid. The 0th element is the raw image
    *                             and the last element is the coarsest image.
    * @param ratio_         [in]  The ratio between each level. It should be in the range [0.4, 0.98].
    * @param num_levels_    [in]  Number of pyramid levels.
    * @param minimum_width_ [in]  Minimum width of the coarsest level.
    *
    * @return Number of levels.
    */
   static int get_image_pyramid_by_levels(
         const cv::Mat& raw_image_,
         std::vector<cv::Mat>& pyramids_,
         float& ratio_,
         int num_levels_,
         int minimum_width_ = 30
   );

   /**
   * Compute the edges using SED.
   *
   * The model file can be downloaded from
   *  https://github.com/csukuangfj/kfj-master-thesis-code-github/blob/master/data/SED/modelFinal.bin.bz2
   *
   * Note that it needs to install the opencv_contrib with my patches,
   * which is available from
   *  https://github.com/csukuangfj/opencv_contrib
   *
   *
   * @param f_        [in]  CV_32FC3, un-normalized image, i.e, maximum pixel value is 255.
   * @param edges_    [out] CV_32FC1, value in range [0,1], which indicates the probability
   *                        of the presence of an edge.
   * @param thinned_  [in]  true to return a thinned edge
   */
   static void compute_edges(
         const cv::Mat& f_,
         cv::Mat& edges_,
         bool thinned_ = false
   );

   /**
    * Bicubic interpolation of a multichannel matrix.
    *
    * Each channel is interpolated separately and their results are
    * returned in a column vector.
    *
    * @param input_  [in] Input image with depth CV_32F
    * @param x_      [in] x-coordinate, i.e., column number, started from 0
    * @param y_      [in] y-coordinate, i.e,, row number, started from 0
    * @return A matrix of size 1-by-1, with input_.channels() channels
    */
   static cv::Mat bicubic_interpolation(
         const cv::Mat& input_,
         float x_,
         float y_
   );

   /**
    * Downsample a multichannel matrix by a given factor.
    *
    * The image size is decreased.
    *
    * @param input_     [in] Input image of depth CV_32F
    * @param factor_    [in] Downsampling factor, it should be in the range (0, 1)
    * @return The downsampled image
    */
   static cv::Mat downsample(
         const cv::Mat& input_,
         float factor_ = 0.5f
   );

   static cv::Mat nearest_neighbor_interpolation(
         const cv::Mat& input_,
         float x_,
         float y_
   );

   static cv::Mat bilinear_interpolation(
         const cv::Mat& input_,
         float x_,
         float y_
   );
   /**
    *
    * @param input_
    * @param x_
    * @param y_
    * @param size_
    * @param mode_  [in] Interpolation mode
    * @return Return a (2*size_+1)*(2*size_+1) patch using bicubic interpolation
    */
   static cv::Mat get_patch(
         const cv::Mat& input_,
         float x_,
         float y_,
         int size_,
         InterpolationMode mode_ = InterpolationMode::E_INTERPOLATION_NN
   );

   //! @sa ::rank_transform()
   static void rank_transform(
         const cv::Mat& in_image_,
         cv::Mat& rank_image_,
         int wnd_size_,
         bool color_to_gray_ = true
   );

   //! @sa ::census_transform()
   static void census_transform(
         const cv::Mat& in_image_,
         cv::Mat& census_image_,
         int wnd_size_,
         bool color_to_gray_ = true
   );

   //! @sa ::complete_rank_transform()
   static void complete_rank_transform(
         const cv::Mat& in_image_,
         cv::Mat& rank_image_,
         int wnd_size_,
         bool color_to_gray_ = true
   );

   //! @sa ::complete_census_transform()
   static void complete_census_transform(
         const cv::Mat& in_image_,
         cv::Mat& rank_image_,
         int wnd_size_,
         bool color_to_gray_ = true
   );
};

#endif //_MY_IMAGEPROCESSING_HPP
