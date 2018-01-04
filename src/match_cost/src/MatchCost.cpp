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
#include "MatchCost.hpp"
#include "SsdCost.hpp"
#include "SadCost.hpp"
#include "HammingCost.hpp"

std::string
match_cost_type_to_string(MatchCostType type_)
{
   std::string res;
   switch (type_)
   {
      case E_COST_TYPE_SSD:
         res = "SSD";
         break;
      case E_COST_TYPE_SAD:
         res = "SAD";
         break;
      case E_COST_TYPE_HAMMING:
         res = "Hamming";
         break;
      default:
         res = "Unknown";
         break;
   }
   return res;
}

cv::Ptr<MatchCost>
MatchCost::create(const cv::String& name_)
{
   cv::Ptr<MatchCost> res;
   cv::String name = name_.toLowerCase();
   if ((name_ == "ssd") || (name_ == "l2"))
   {
      res = cv::makePtr<SsdCost>();
   }
   else if ((name == "sad") || (name == "l1"))
   {
      res = cv::makePtr<SadCost>();
   }
   else if (name == "hamming")
   {
      res = cv::makePtr<HammingCost>();
   }

   return res;
}

cv::Ptr<MatchCost>
MatchCost::create(MatchCostType type_)
{
   cv::Ptr<MatchCost> res;
   switch (type_)
   {
      case E_COST_TYPE_SSD:
         res = cv::makePtr<SsdCost>();
         break;
      case E_COST_TYPE_SAD:
         res = cv::makePtr<SadCost>();
         break;
      case E_COST_TYPE_HAMMING:
         res = cv::makePtr<HammingCost>();
         break;
      default:
         CV_Assert(false); // unreachable code
         break;

   }

   return res;
}
