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
#include <opencv2/imgcodecs.hpp>

#include "StErrorStatistics.hpp"

StErrorStatistics::StErrorStatistics()
   :
      m_all(-1),
      m_disc(-1),
      m_nonocc(-1)
{}

void
StErrorStatistics::evaluate_MiddleburyV2(
      const cv::Mat &ground_truth_,
      const cv::Mat &estimated_disaprity_,
      const cv::Mat &mask_all_,
      const cv::Mat &mask_disc_,
      const cv::Mat &mask_nonocc,
      float scale_,
      float threshold_
)
{
   CV_Assert(ground_truth_.type() == estimated_disaprity_.type());
   CV_Assert(ground_truth_.type() == mask_all_.type());
   CV_Assert(ground_truth_.type() == mask_disc_.type());
   CV_Assert(ground_truth_.type() == mask_nonocc.type());
   CV_Assert(ground_truth_.type() == CV_8UC1);

   CV_Assert(ground_truth_.size() == estimated_disaprity_.size());
   CV_Assert(ground_truth_.size() == mask_all_.size());
   CV_Assert(ground_truth_.size() == mask_disc_.size());
   CV_Assert(ground_truth_.size() == mask_nonocc.size());

   int ny = ground_truth_.rows;
   int nx = ground_truth_.cols;

   int total_number_all = 0;
   int total_number_disc = 0;
   int total_number_nonocc = 0;

   int invalid_number_all = 0;
   int invalid_number_disc = 0;
   int invalid_number_nonocc = 0;

   float scaled_threshold = scale_ * threshold_;
   for (int y = 0; y < ny; y++)
   {
      for (int x = 0; x < nx; x++)
      {
         float diff = ground_truth_.at<uchar>(y,x) - estimated_disaprity_.at<uchar>(y,x);
         diff = cv::abs(diff);

         if (mask_all_.at<uchar>(y,x) == 255)
         {
            total_number_all++;
            if (diff > scaled_threshold)
            {
               invalid_number_all++;
            }
         }

         if (mask_disc_.at<uchar>(y,x) == 255)
         {
            total_number_disc++;
            if (diff > scaled_threshold)
            {
               invalid_number_disc++;
            }
         }

         if (mask_nonocc.at<uchar>(y,x) == 255)
         {
            total_number_nonocc++;
            if (diff > scaled_threshold)
            {
               invalid_number_nonocc++;
            }
         }
      }
   } // end for (int y

   if (total_number_all)
   {
      m_all = (float)invalid_number_all / total_number_all * 100;
   }

   if (total_number_disc)
   {
      m_disc = (float)invalid_number_disc / total_number_disc * 100;
   }

   if (total_number_nonocc)
   {
      m_nonocc = (float)invalid_number_nonocc / total_number_nonocc * 100;
   }
}

void
StErrorStatistics::evaluate_MiddleburyV2(
      const cv::String &ground_truth_filename_,
      const cv::String &estimated_disparity_filename_,
      const cv::String &mask_all_filename_,
      const cv::String &mask_disc_filename_,
      const cv::String &mask_nonocc_filename_,
      float scale_,
      float threshold_
)
{
   cv::Mat ground_truth;
   cv::Mat estimated_disparity;
   cv::Mat mask_all;
   cv::Mat mask_disc;
   cv::Mat mask_nonocc;

   ground_truth = cv::imread(ground_truth_filename_, cv::IMREAD_GRAYSCALE);
   estimated_disparity = cv::imread(estimated_disparity_filename_, cv::IMREAD_GRAYSCALE);
   mask_all = cv::imread(mask_all_filename_, cv::IMREAD_GRAYSCALE);
   mask_disc = cv::imread(mask_disc_filename_, cv::IMREAD_GRAYSCALE);
   mask_nonocc = cv::imread(mask_nonocc_filename_, cv::IMREAD_GRAYSCALE);

   CV_Assert(!ground_truth.empty());
   CV_Assert(!estimated_disparity.empty());
   CV_Assert(!mask_all.empty());
   CV_Assert(!mask_disc.empty());
   CV_Assert(!mask_nonocc.empty());

   evaluate_MiddleburyV2(ground_truth, estimated_disparity, mask_all, mask_disc, mask_nonocc, scale_, threshold_);
}

std::string
StErrorStatistics::to_string() const
{
   cv::String s = cv::format(
         "nonocc: %.2f\n"
         "all: %.2f\n"
         "disc: %.2f\n",
         m_nonocc,
         m_all,
         m_disc
   );
   return s;
}
