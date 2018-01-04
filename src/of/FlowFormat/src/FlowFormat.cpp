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

#include "FlowFormat.hpp"
#include "FlowFormat_F.hpp"
#include "FlowFormat_FLO.hpp"
#include "FlowFormat_KITTI.hpp"

cv::Ptr<FlowFormat>
FlowFormat::create(const std::string& name_)
{
   cv::Ptr<FlowFormat> res;

   if ((name_ == "F") || (name_ == "f"))
   {
      res = cv::makePtr<FlowFormat_F>();
   }
   else if ((name_ == "flo") || (name_ == "FLO"))
   {
      res = cv::makePtr<FlowFormat_FLO>();
   }
   else if ((name_ == "kitti") || (name_ == "KITTI") || (name_ == "png"))
   {
      res = cv::makePtr<FlowFormat_KITTI>();
   }
   else
   {
      auto n = name_.rfind('.');
      if (n != std::string::npos)
      {
         res = create(name_.substr(n+1));
      }
   }

   return res;
}

cv::Ptr<FlowFormat>
FlowFormat::create(Format format_)
{
   cv::Ptr<FlowFormat> res;
   switch(format_)
   {
      case E_FLOW_FORMAT_F:
         res = cv::makePtr<FlowFormat_F>();
         break;
      case E_FLOW_FORMAT_FLO:
         res = cv::makePtr<FlowFormat_FLO>();
         break;
      case E_FLOW_FORMAT_KITTI:
         res = cv::makePtr<FlowFormat_KITTI>();
         break;
      default:
         CV_Assert(false); // unreachable code
         break;
   }
   return res;
}
