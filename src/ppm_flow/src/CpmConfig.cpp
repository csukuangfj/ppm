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

#include "CpmConfig.hpp"

CpmConfig::CpmConfig()
   : m_grid_space(3),
     m_pyramid_ratio(0.75),
     m_num_pyramid_level(8),
     m_max_displacement(400),
     m_num_iterations(8),
     m_half_patch_size(0),
     m_minimum_image_width(30),
     m_cross_check_enabled(true),

     m_verbose(true),

     m_descriptor_type(DescriptorType::E_DESC_TYPE_SIFT),
     m_descriptor_color_to_gray(true),
     m_match_cost_type(E_COST_TYPE_SAD),
     m_pm_property_type(PmPropertyType::E_PROPERTY_FLOW),

     m_use_interpolation(true)
{}

std::string
CpmConfig::pm_property_type_to_string(PmPropertyType type_)
{
   std::string res;
   switch (type_)
   {
      case PmPropertyType::E_PROPERTY_FLOW:
         res = "Flow (Translational model)";
         break;
      case PmPropertyType ::E_PROPERTY_AFFINE_MODEL:
         res = "Affine model";
         break;
      case PmPropertyType::E_PROPERTY_QUADRATIC_MODEL:
         res = "Quadratic model";
         break;
      case PmPropertyType::E_PROPERTY_PROJECTIVE_PLANAR:
         res = "Projective planar";
         break;
      default:
         CV_Assert(false);  // unreachable code
         break;
   }
   return res;
}

std::string
CpmConfig::to_string() const
{
   std::stringstream ss;
   ss << std::endl
      << "Grid space: " << m_grid_space << std::endl
      << "Pyramid ratio: " << m_pyramid_ratio << std::endl
      << "Number of pyramid levels: " << m_num_pyramid_level << std::endl
      << "Max displacement: " << m_max_displacement << std::endl
      << "Number of iterations: " << m_num_iterations << std::endl
      << "Half patch size: " << m_half_patch_size << std::endl
      << "Minimum image width: " << m_minimum_image_width << std::endl
      << "Cross check: " << (m_cross_check_enabled ? "true" : "false" ) << std::endl
      << "Verbose: " << (m_verbose ? "true" : "false" ) << std::endl
      << "Descriptor type: " << descriptor_type_to_string(m_descriptor_type) << std::endl
      << "Descriptor color to gray: " << (m_descriptor_color_to_gray ? "true" : "false") << std::endl
      << "Match cost type: " << match_cost_type_to_string(m_match_cost_type) << std::endl
      << "Property type: " << pm_property_type_to_string(m_pm_property_type) << std::endl
      << "Use interpolation: " << (m_use_interpolation ? "true" : "false" ) << std::endl
      ;
   if (!m_filename_1.empty())
   {
      ss << "frame 1: " << m_filename_1 << std::endl
         << "frame 2: " << m_filename_2 << std::endl;
   }

   if (!m_gt_filename.empty())
   {
      ss << "ground truth filename: " << m_gt_filename << std::endl;
   }

   ss << std::endl;

   return ss.str();
}

void
CpmConfig::show_parameters() const
{
   std::cout << to_string();
}
