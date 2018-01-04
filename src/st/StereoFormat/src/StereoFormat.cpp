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
#include "StereoFormat.hpp"
#include "StereoFormatPfm.hpp"

cv::Ptr<StereoFormat>
StereoFormat::create(Format format_)
{
   cv::Ptr<StereoFormat> res;
   switch(format_)
   {
      case Format::E_PFM:
         res = cv::makePtr<StereoFormatPfm>();
         break;
      default:
         CV_Assert(false); // unreachable code
         break;
   }
   return res;
}

cv::Ptr<StereoFormat>
StereoFormat::create(const cv::String &filename_)
{
   cv::Ptr<StereoFormat> res;
   if ((filename_ == "pfm") || (filename_ == "PFM") || (filename_ == "Pfm"))
   {
      res = cv::makePtr<StereoFormatPfm>();
   }
   else
   {
      auto n = filename_.rfind('.');
      if (n != std::string::npos)
      {
         res = create(filename_.substr(n+1));
      }
   }

   return res;
}

