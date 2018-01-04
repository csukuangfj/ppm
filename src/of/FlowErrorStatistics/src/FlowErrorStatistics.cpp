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
#include <iostream>
#include <sstream>
#include <cmath>

#include <opencv2/core.hpp>

#include "FlowErrorStatistics.hpp"

#define UNKNOWN_FLOW_THRESH 1e9
#define UNKNOWN_FLOW 1e10

FlowErrorStatistics::FlowErrorStatistics()
   : m_AAE(-1),
     m_AEE(-1),
     m_ABP(-1),
     m_bad_pixel_threshold(3)
{}

void
FlowErrorStatistics::compute_error_statistics(
      const cv::Mat& u_t_,
      const cv::Mat& v_t_,
      const cv::Mat& u_e_,
      const cv::Mat& v_e_,
      cv::InputArray mask_
)
{
   CV_Assert(u_t_.size() == v_t_.size());
   CV_Assert(u_e_.size() == v_e_.size());
   CV_Assert(u_t_.size() == u_e_.size());

   CV_Assert(u_t_.type() == v_t_.type());
   CV_Assert(u_e_.type() == v_e_.type());
   CV_Assert(u_t_.type() == u_e_.type());

   CV_Assert(u_t_.type() == CV_32FC1);

   cv::Mat mask;
   mask = mask_.getMat();
   CV_Assert(mask.empty() ||
             (mask.type() == CV_8UC1 && u_t_.size() == mask.size()));

   if (mask.empty())
   {
      mask = cv::Mat::ones(u_t_.size(), CV_8UC1)*255;
   }

   m_AAE = 0.0f;
   m_AEE = 0.0f;
   m_ABP = 0.0f;

   int ny = u_t_.rows;
   int nx = u_t_.cols;

   int n = 0; // number of pixels considered
   for (int y = 0; y < ny; y++)
   {
      const float *put = u_t_.ptr<float>(y);
      const float *pvt = v_t_.ptr<float>(y);

      const float *pue = u_e_.ptr<float>(y);
      const float *pve = v_e_.ptr<float>(y);

      const uchar *pmask = mask.ptr<uchar>(y);
      for (int x = 0; x < nx; x++)
      {
         double ut,ue,vt,ve;
         ut = put[x]; vt = pvt[x];
         ue = pue[x]; ve = pve[x];
         if (pmask[x])
         {
            if ((cv::abs(ut) > UNKNOWN_FLOW_THRESH)
                  || (cv::abs(vt) > UNKNOWN_FLOW_THRESH)
                   || (cv::abs(ue) > UNKNOWN_FLOW_THRESH)
                   || (cv::abs(ve) > UNKNOWN_FLOW_THRESH)
                  )
            {
               continue;
            }

            double numerator = ut*ue + vt*ve + 1;
            double denominator = std::sqrt(ut*ut+vt*vt+1) * std::sqrt(ue*ue+ve*ve+1);
            double tmp = numerator/denominator;
            if (tmp > 1) tmp = 1;
            if (tmp < -1) tmp = -1;

            m_AAE += std::acos(tmp); // acos returns radian instead of degree!

            double bp;

            bp = std::sqrt((ut-ue)*(ut-ue) + (vt-ve)*(vt-ve));

            m_AEE += bp;
            if (bp > m_bad_pixel_threshold)
            {
               m_ABP++;
            }

            n++;
         }
      } // for (int x = 0; x < nx; x++)
   } // for (int y = 0; y < ny; y++)

   if (n)
   {
      m_AAE /= n;   // average
      m_AAE = m_AAE * 180 * (float)M_1_PI; // convert into degree

      m_AEE /= n;   // average
      m_ABP /= n;   // average
   }
   else
   {
      std::cerr << "Mask contains all 0s!" << std::endl;
      m_AAE = -1;
      m_AEE = -1;
      m_ABP = -1;
   }
}

cv::String
FlowErrorStatistics::to_string() const
{
   cv::String res;
   res = cv::format("AAE: %.3f\n"
                    "AEE: %.3f\n"
                    "BP: %.3f%%\n",
                    m_AAE, m_AEE, m_ABP*100);
   return res;
}