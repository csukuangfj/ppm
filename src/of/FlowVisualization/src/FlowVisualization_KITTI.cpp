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
#include "FlowVisualization_KITTI.hpp"
#include <iostream>

/**
 *
 * @param h_   [in] Should it be [0,1]
 * @param s_   [in]
 * @param v_   [in]
 * @param r_   [out]
 * @param g_   [out]
 * @param b_   [out]
 *
 */
static inline void
hsv_to_rgb (float h_, float s_, float v_, float &r_, float &g_, float &b_)
{
   float c  = v_*s_;
   float h2 = 6.0f*h_;
   float x  = c*(1.0f-(float)fabs(fmod(h2,2.0f)-1.0f));
   if (0<=h2&&h2<1)       { r_ = c; g_ = x; b_ = 0; }
   else if (1<=h2&&h2<2)  { r_ = x; g_ = c; b_ = 0; }
   else if (2<=h2&&h2<3)  { r_ = 0; g_ = c; b_ = x; }
   else if (3<=h2&&h2<4)  { r_ = 0; g_ = x; b_ = c; }
   else if (4<=h2&&h2<5)  { r_ = x; g_ = 0; b_ = c; }
   else if (5<=h2&&h2<=6) { r_ = c; g_ = 0; b_ = x; }
   else if (h2>6) { r_ = 1; g_ = 0; b_ = 0; }
   else if (h2<0) { r_ = 0; g_ = 1; b_ = 0; }
}

void
FlowVisualization_KITTI::flow_to_image(
     const cv::Mat &u_,
     const cv::Mat &v_,
     cv::Mat &image_,
     int max_disp_,
     const cv::Mat &mask_
)
{
   CV_Assert(u_.size == v_.size);
   CV_Assert(u_.type() == v_.type());
   CV_Assert(u_.type() == CV_32FC1);

   CV_Assert(mask_.empty() ||
            ((mask_.type() == CV_8UC1) && (mask_.size() == u_.size())));
   cv::Mat mask = mask_;
   if (mask.empty())
   {
     mask = cv::Mat::ones(u_.size(), CV_8UC1);
   }

   image_ = cv::Mat::zeros(u_.size(), CV_8UC3);

   if (max_disp_ <= 0)
   {
     cv::Mat tmp;
     cv::magnitude(u_, v_, tmp);

     double max_val;
     cv::minMaxIdx(tmp, nullptr, &max_val);
     max_disp_ = (int)max_val;
   }

   float scaling = 1.0f/max_disp_;

   int nr = u_.rows;
   int nc = u_.cols;

   const cv::Mat_<float>& u_ref = (const cv::Mat_<float>&)u_;
   const cv::Mat_<float>& v_ref = (const cv::Mat_<float>&)v_;
   const cv::Mat_<uchar>& mask_ref = (const cv::Mat_<uchar>&)mask;

   cv::Mat_<cv::Vec3b>& out_ref = (cv::Mat_<cv::Vec3b>&)image_;

   float n = 8; // multiplier

   for(int j = 0; j < nr; j++)
   {
      for(int i = 0; i < nc; i++)
      {
         float u_ij = u_ref(j,i) * scaling;
         float v_ij = v_ref(j,i) * scaling;
         if (!mask_ref(j,i))
         {
            continue;
         }

         float r = 0;
         float g = 0;
         float b = 0;

         float mag = cv::sqrt(u_ij*u_ij + v_ij*v_ij);
         float dir = (float)atan2(v_ij, u_ij);
         float h   = (float)fmod(dir/(2.0*M_PI)+1.0, 1.0);
         float s   = std::min(std::max(mag*n*scaling, 0.0f), 1.0f);
         float v   = std::min(std::max(n-s, 0.0f), 1.0f);
         hsv_to_rgb(h, s, v, r, g, b);
         out_ref(j,i) = cv::Vec3b(cv::saturate_cast<uchar>(b*255),
                                  cv::saturate_cast<uchar>(g*255),
                                  cv::saturate_cast<uchar>(r*255));
      }
   }
}
