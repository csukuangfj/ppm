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
#ifndef _StErrorStatistics_HPP_
#define _StErrorStatistics_HPP_

#include <opencv2/core.hpp>

class StErrorStatistics
{
public:
   StErrorStatistics();

   /**
    *
    * @param m_ground_truth_        [in] CV_8UC1
    * @param m_estimated_disaprity_
    * @param m_mask_all_
    * @param m_mask_dis_
    * @param m_mask_nonocc
    * @param scale_                 [in] teddy - 4
    *                                    cones - 4
    *                                    venus - 8
    *                                    tsukuba - 16
    *
    * @param threshold_             [in] 0.5, 0.75, 1, 1.5, 2
    */
   void evaluate_MiddleburyV2(
         const cv::Mat& ground_truth_,
         const cv::Mat& estimated_disparity_,
         const cv::Mat& mask_all_,
         const cv::Mat& mask_disc_,
         const cv::Mat& mask_nonocc,
         float scale_,
         float threshold_
   );

   void evaluate_MiddleburyV2(
         const cv::String& ground_truth_filename_,
         const cv::String& estimated_disparity_filename_,
         const cv::String& mask_all_filename_,
         const cv::String& mask_disc_filename_,
         const cv::String& mask_nonocc_filename_,
         float scale_,
         float threshold_
   );

   std::string to_string() const;

public:
   //getters
   float get_all() const {return m_all;}
   float get_disc() const {return m_disc;}
   float get_nonocc() const {return m_nonocc;}

private:
   float m_all;    //!< all, percent, 2.3 means 2.3%
   float m_disc;   //!< depth discontinuities, percent, 2.3 means 2.3%
   float m_nonocc; //!< non-occluded, percent, 2.3 means 2.3%
};

#endif //_StErrorStatistics_HPP_
