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
#include "PatchMatchStereoImpl.hpp"

#include "PatchMatchStereoImplSlantedPlane.hpp"
#include "PatchMatchStereoImplTranslationalModel.hpp"
#include "PatchMatchStereoImplProjectivePlanar.hpp"

cv::Ptr<PatchMatchStereoImpl>
PatchMatchStereoImpl::create(
      PatchMatchStereoSlantedConfig::PropertyType type_
)
{
   cv::Ptr<PatchMatchStereoImpl> res;
   switch (type_)
   {
      case PatchMatchStereoSlantedConfig::PropertyType::E_SLANTED_PLANE:
         res = cv::makePtr<PatchMatchStereoImplSlantedPlane>();
         break;
      case PatchMatchStereoSlantedConfig::PropertyType::E_TRANSLATIONAL_MODEL:
         res = cv::makePtr<PatchMatchStereoImplTranslationalModel>();
         break;
      case PatchMatchStereoSlantedConfig::PropertyType::E_PROJECTIVE_PLANAR:
         res = cv::makePtr<PatchMatchStereoImplProjectivePlanar>();
         break;
      default:
         CV_Assert(false); // unreachable code
         break;
   }

   return res;
}
