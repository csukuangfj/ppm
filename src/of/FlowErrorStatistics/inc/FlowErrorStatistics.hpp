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
#ifndef __FLOWERRORSTATISTICS_HPP__
#define __FLOWERRORSTATISTICS_HPP__

#include <opencv2/core.hpp>

/**
 * This class computes the following statistics:
 *  - AAE (Average Angular Error) in degrees
 *  - AEE (Average Endpoint Error)
 *  - ABP (Average Bad Pixel)
 *
 *  Refer to:
 *    Baker, Simon, et al. "A database and evaluation methodology for optical flow."
 *    International Journal of Computer Vision 92.1 (2011): 1-31.
 *    (https://link.springer.com/content/pdf/10.1007/s11263-010-0390-2.pdf)
 *
 *  for the definition of **AAE** (page 16, equation 22) and **AEE** (page 16, equation 23).
 *
 *  ABP is used in KITTI. Refer to <http://www.cvlibs.net/datasets/kitti/eval_scene_flow.php>
 *  The default threshold is 3-pixel
 */
class FlowErrorStatistics
{
public:
   FlowErrorStatistics();

   /**
    *  Compute error statistics using the estimated flow and the ground truth.
    *
    * @param u_t_  [in] Ground truth flow in x-direction, CV_32FC1
    * @param v_t_  [in] Ground truth flow in y-direction, CV_32FC1
    * @param u_e_  [in] Estimated flow in x-direction, CV_32FC1
    * @param v_e_  [in] Estimated flow in y-direction, CV_32FC1
    * @param mask_ [in] Mask, CV_8UC1
    */
   void compute_error_statistics(
         const cv::Mat& u_t_,
         const cv::Mat& v_t_,
         const cv::Mat& u_e_,
         const cv::Mat& v_e_,
         cv::InputArray mask_ = cv::noArray()
   );

   double get_AAE() const {return m_AAE;}
   double get_AEE() const {return m_AEE;}
   double get_ABP() const {return m_ABP;}

   void set_bad_pixel_threshold(float bp_threshold_) {m_bad_pixel_threshold = bp_threshold_;}
   float get_bad_pixel_threshold() const {return m_bad_pixel_threshold;}

   cv::String to_string() const;

private:
   double m_AAE; //!< Average Angular Error rate in degrees
   double m_AEE; //!< Average Endpoint Error
   double m_ABP; //!< Average Bad pixel Error

   float m_bad_pixel_threshold;
};

#endif //__FLOWERRORSTATISTICS_HPP__
