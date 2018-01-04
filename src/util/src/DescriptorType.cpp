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
#include <string>
#include "DescriptorType.hpp"

std::string
descriptor_type_to_string(DescriptorType type_)
{
   std::string res;
   switch (type_)
   {
      case DescriptorType::E_DESC_TYPE_SIFT:
         res = "sift";
         break;
      case DescriptorType::E_DESC_TYPE_RANK_TRANSFORM:
         res = "rank transform";
         break;
      case DescriptorType::E_DESC_TYPE_CENSUS_TRANSFORM:
         res = "census transform";
         break;
      case DescriptorType::E_DESC_TYPE_COMPLETE_RANK_TRANSFORM:
         res = "complete rank transform";
         break;
      case DescriptorType::E_DESC_TYPE_COMPLETE_CENSUS_TRANSFORM:
         res = "complete census transform";
         break;
      default:
         res = "unknown descriptor type";
         break;
   }
   return res;
}
