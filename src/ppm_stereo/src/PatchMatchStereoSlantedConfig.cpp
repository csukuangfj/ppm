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
#include <opencv2/core.hpp>
#include <sstream>

#include "PatchMatchStereoSlantedConfig.hpp"

PatchMatchStereoSlantedConfig::PatchMatchStereoSlantedConfig()
   : m_iterations(3),
     m_half_patch_size(17),
     m_gamma_color(10.0f),
     m_alpha(0.9f),
     m_tau_color(10),
     m_tau_gradient(2),

     m_max_disparity(60),
     m_max_delta_z0(m_max_disparity),
     m_max_delta_nxyz(1.0f),

     m_scale_disparity(1.0f),

     m_property_type(PropertyType::E_SLANTED_PLANE),
     m_output_directory("/tmp"),
     m_verbose(true)
{}

void
PatchMatchStereoSlantedConfig::init(
      int iterations_,
      int half_patch_size_,
      float gamma_color_,
      float alpha_,
      float tau_color_,
      float tau_gradient_,
      int max_disparity_,
      float scale_disparity_
)
{
   m_iterations = iterations_;
   m_half_patch_size = half_patch_size_;
   m_gamma_color = gamma_color_;
   m_alpha = alpha_;
   m_tau_color = tau_color_;
   m_tau_gradient = tau_gradient_;

   m_max_disparity = max_disparity_;
   m_max_delta_z0 = m_max_disparity;
   m_max_delta_nxyz = 1.0f;
   m_scale_disparity = scale_disparity_;
}

std::string
PatchMatchStereoSlantedConfig::property_type_to_string() const
{
   std::string res;
   switch (m_property_type)
   {
      case PropertyType::E_SLANTED_PLANE:
         res = "Slanted plane";
         break;
      case PropertyType::E_TRANSLATIONAL_MODEL:
         res = "Translational model";
         break;
      case PropertyType::E_PROJECTIVE_PLANAR:
         res = "Projective planar model";
         break;
      default:
         CV_Assert(false); // unreachable code
         break;
   }
   return res;
}

std::string
PatchMatchStereoSlantedConfig::to_string() const
{
   std::stringstream  ss;
   ss << "\n"
      << "Number of iterations: " << m_iterations << "\n"
      << "Half patch size: " << m_half_patch_size << "\n"
      << "Gamma color: " << m_gamma_color << "\n"
      << "Alpha: " << m_alpha << "\n"
      << "tau color: " << m_tau_color << "\n"
      << "tau gradient: " << m_tau_gradient << "\n"
      << "Max disparity: " << m_max_disparity << "\n"
      //<< "max delta z: " << m_max_delta_z0 << "\n"
      << "max delta n: " << m_max_delta_nxyz << "\n"
      << "Disparity scale: " << m_scale_disparity << "\n"
      << "Property type: " << property_type_to_string() << "\n"
      << "Output directory: " << m_output_directory << "\n"
      << "Verbose: " << (m_verbose ? "true" : "false") << "\n"
     ;
   return ss.str();
}
