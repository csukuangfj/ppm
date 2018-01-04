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
#include <opencv2/core.hpp>

#include "FlowVisualization.hpp"
#include "FlowVisualization_FLO.hpp"
#include "FlowVisualization_KITTI.hpp"

static void
linspace(cv::Mat& m_, float low_, float high_, int n_)
{
   CV_DbgAssert(n_ > 1)
   m_.create(n_, 1, CV_32FC1);
   float step = (high_ - low_)/(n_-1);
   float* p = m_.ptr<float>(0);
   for (int i = 0; i < n_; ++i)
   {
      p[i] = low_ + i*step;
   }
}

void
FlowVisualization::get_standard_color_wheel(cv::Mat &image_)
{
   float r = 1.05f;
   int n = 300;
   cv::Mat x;
   linspace(x, -r, r, n);

   cv::Mat u;
   cv::Mat v;
   u = repeat(x, 1, n);
   v = repeat(x.t(), n, 1);

   cv::Mat_<float>& uu = (cv::Mat_<float>&)u;
   cv::Mat_<float>& vv = (cv::Mat_<float>&)v;
   for (int j = 0; j < u.rows; ++j)
   {
      for (int i = 0; i < u.cols; ++i)
      {
         float t1 = uu(j,i);
         float t2 = vv(j,i);
         if (t1*t1 + t2*t2 > r)
         {
            uu(j,i) = 0; // background is black
            vv(j,i) = 0;
         }
      }
   }

   flow_to_image(u, v, image_, (int)(r*1.2f));
}

cv::Ptr<FlowVisualization>
FlowVisualization::create(const std::string& name_)
{
   cv::Ptr<FlowVisualization> res;
   if ((name_ == "FLO") || (name_ == "flo") || (name_ == "Middlebury"))
   {
      res = cv::makePtr<FlowVisualization_FLO>();
   }
   else if ((name_ == "KITTI") || (name_ == "kitti"))
   {
      res = cv::makePtr<FlowVisualization_KITTI>();
   }

   return res;
}

cv::Ptr<FlowVisualization>
FlowVisualization::create(Format format_)
{
   cv::Ptr<FlowVisualization> res;
   switch(format_)
   {
      case E_FLOW_VISULIZATION_FLO:
         // fall through
      case E_FLOW_VISULIZATION_MIDDLEBURY:
         res = cv::makePtr<FlowVisualization_FLO>();
         break;
      case E_FLOW_VISULIZATION_KITTI:
         res = cv::makePtr<FlowVisualization_KITTI>();
         break;
      default:
         CV_Assert(false); // unreachable code
         break;
   }

   return res;
}
