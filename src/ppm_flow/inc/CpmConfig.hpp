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
#ifndef __CPMCONFIG_HPP__
#define __CPMCONFIG_HPP__

#include "DescriptorType.hpp"
#include "MatchCost.hpp"

class CpmConfig
{
public:

   //enum class PmPropertyType
   enum PmPropertyType
   {
      E_PROPERTY_FLOW = 0,                //!< property is a flow field
      E_PROPERTY_AFFINE_MODEL = 1,        //!< property is an affine model
      E_PROPERTY_QUADRATIC_MODEL = 2,     //!< quadratic model
      E_PROPERTY_PROJECTIVE_PLANAR = 3,   //!< projective planar
   };

   //enum class ViewIndex // doxygen does not support enum class
   enum ViewIndex
   {
      E_LEFT_VIEW    = 0, //!< first frame, reference frame
      E_RIGHT_VIEW   = 1, //!< second frame
   };

public:
   CpmConfig();

   void set_grid_space(int val_) {m_grid_space = val_;}
   int get_grid_space() const {return m_grid_space;}

   void set_pyramid_ratio(float val_) {m_pyramid_ratio = val_;}
   float get_pyramid_ratio() const {return m_pyramid_ratio;}

   void set_number_of_pyramid_levels(int val_) {m_num_pyramid_level = val_;}
   int get_number_of_pyramid_levels() const {return m_num_pyramid_level;}

   void set_max_displacement(int val_) {m_max_displacement = val_;}
   int get_max_displacement() const {return m_max_displacement;}

   void set_number_of_iterations(int val_) {m_num_iterations = val_;}
   int get_number_of_iterations() const {return m_num_iterations;}

   void set_half_patch_size(int val_) {m_half_patch_size = val_;}
   int get_half_patch_size() const {return m_half_patch_size;}

   void set_minimum_image_width(int val_) {m_minimum_image_width = val_;}
   int get_minimum_image_width() const {return m_minimum_image_width;}

   void set_cross_check(bool val_) {m_cross_check_enabled = val_;}
   bool get_cross_check() const {return m_cross_check_enabled;}

   void set_verbose(bool verbose_) {m_verbose = verbose_;}
   bool get_verbose() const {return m_verbose;}

   void set_descriptor_type(DescriptorType type_) {m_descriptor_type = type_;}
   DescriptorType get_descriptor_type() const {return m_descriptor_type;}

   void set_descriptor_color_to_gray(bool color_to_gray_) {m_descriptor_color_to_gray = color_to_gray_;}
   bool get_descriptor_color_to_gray() const {return m_descriptor_color_to_gray;}

   void set_match_cost_type(MatchCostType type_) {m_match_cost_type = type_;}
   MatchCostType get_match_cost_type() const {return m_match_cost_type;}

   void set_pm_property_type(PmPropertyType val_) {m_pm_property_type = val_;}
   PmPropertyType get_pm_property_type() const {return m_pm_property_type;}

   static std::string pm_property_type_to_string(PmPropertyType type_);

   std::string to_string() const;
   void show_parameters() const;

   void set_filename1(const std::string& val_) {m_filename_1 = val_;}
   std::string get_filename1() const {return m_filename_1;}

   void set_filename2(const std::string& val_) {m_filename_2 = val_;}
   std::string get_filename2() const {return m_filename_2;}

   void set_ground_truth_filename(const std::string& val_) {m_gt_filename = val_;}
   std::string get_ground_truth_filename() const {return m_gt_filename;}

   void set_use_interpolation(bool val_) {m_use_interpolation = val_;}
   bool get_use_interpolation() const {return m_use_interpolation;}

private:
   int m_grid_space;    //!< Grid space between seeds.
                        //!< The horizontal space and the vertical space are equal.

   float m_pyramid_ratio;  //!< Size ratio between consecutive levels.

   int m_num_pyramid_level;    //!< Number of pyramid levels. When it is less than 1,
                               //!< it computes as many levels as possible.
   int m_max_displacement;     //!< Maximum displacement

   int m_num_iterations;      //!< Number of iterations in PatchMatch Propagation step

   int m_half_patch_size;     //!< half patch size for computing match cost.
                              //!< 0 means to compute point-wise cost, i.e, 1x1.
                              //!< It should be non-negative.

   int m_minimum_image_width; //!< The minimum width of the coarsest level in the pyramid

   bool m_cross_check_enabled; //!< true to enable cross check, false to disable cross check

   bool m_verbose; //!< true to display more debug information, false otherwise

   DescriptorType m_descriptor_type;  //!< descriptor type
   bool m_descriptor_color_to_gray;    //!< true to convert the color image to grayscale image, and then compute the
                                       //!< descriptor of the grayscale image
                                       //!< false to compute the descriptor of each channel separately and then
                                       //!< concatenate them

   MatchCostType m_match_cost_type;   //!< match cost type

   PmPropertyType m_pm_property_type; //!< property type, i.e, model type in the thesis

private:
   std::string m_filename_1;
   std::string m_filename_2;
   std::string m_gt_filename;

   bool m_use_interpolation;
};

#endif //__CPMCONFIG_HPP__
