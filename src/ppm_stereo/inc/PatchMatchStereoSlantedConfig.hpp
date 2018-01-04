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
#ifndef _PATCHMATCHSTEREOSLANTEDCONFIG_HPP_
#define _PATCHMATCHSTEREOSLANTEDCONFIG_HPP_

#include <string>
#include <opencv2/core.hpp>

class PatchMatchStereoSlantedConfig
{
public:
   enum class PropertyType
   {
      E_SLANTED_PLANE   = 0,     //!< slanted support window
      E_TRANSLATIONAL_MODEL = 1, //!< translational, this corresponds to the epiploar model in the thesis
      E_PROJECTIVE_PLANAR = 3,   //!< projective planar
   };

public:
   PatchMatchStereoSlantedConfig();

   void init(
         int iterations_,
         int half_patch_size_,
         float gamma_color_,
         float alpha_,
         float tau_color_,
         float tau_gradient_,
         int max_disparity_,
         float scale_disparity_
   );

   void set_number_of_iterations(int iterations_) {m_iterations = iterations_;}
   int get_number_of_iterations() const {return m_iterations;}

   void set_half_patch_size(int half_size) {m_half_patch_size = half_size;}
   int get_half_patch_size() const {return m_half_patch_size;}

   void set_gamma_color(float gamma_color_) {m_gamma_color = gamma_color_;}
   float get_gamma_color() const {return m_gamma_color;}

   void set_alpha(float alpha_) {m_alpha = alpha_;}
   float get_alpha() const {return m_alpha;}

   void set_tau_color(float tau_color_) {m_tau_color = tau_color_;}
   float get_tau_color() const {return m_tau_color;}

   void set_tau_gradient(float tau_grad_) {m_tau_gradient = tau_grad_;}
   float get_tau_gradient() const {return m_tau_gradient;}

   void set_max_disparity(int disparity_) {m_max_disparity = disparity_;}
   int get_max_disparity() const {return m_max_disparity;}

   void set_max_delta_z0(float z0_) {m_max_delta_z0 = z0_;}
   float get_max_delta_z0() const {return m_max_delta_z0;}

   void set_max_delta_nxyz(float n_) {m_max_delta_nxyz = n_;}
   float get_max_delta_nxyz() const {return m_max_delta_nxyz;}

   void set_disparity_scale(float scale_) {m_scale_disparity = scale_;}
   float get_disparity_scale() const {return m_scale_disparity;}

   std::string property_type_to_string() const;
   std::string to_string() const;

   void set_property_type(PropertyType val_) {m_property_type = val_;}
   PropertyType get_property_type() const {return m_property_type;}

   void set_output_directory(const cv::String& val_) {m_output_directory = val_;}
   cv::String get_output_directory() const {return m_output_directory;}

   void set_verbose(bool val_) {m_verbose = val_;}
   bool get_verbose() const {return m_verbose;}

private:
   int m_iterations;      //!< number of iterations
   int m_half_patch_size; //!< half patch size, the whole patch size is (2*m_half_patch_size+1)x(2*half_patch_size+1)
   float m_gamma_color;
   float m_alpha;
   float m_tau_color;
   float m_tau_gradient;

   /*
    tsukuba: 16
    venus: 20
    teddy: 60
    cones: 60
    */
   int m_max_disparity;
   float m_max_delta_z0;
   float m_max_delta_nxyz;

   /*
    tsukuba: 16
    venus: 8
    teddy: 4
    cones: 4
    */
   float m_scale_disparity;

   //
   PropertyType m_property_type;

   cv::String m_output_directory;

   bool m_verbose;
};

#endif //_PATCHMATCHSTEREOSLANTEDCONFIG_HPP_
