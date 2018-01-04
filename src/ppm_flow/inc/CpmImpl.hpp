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
#ifndef _CpmImpl_HPP_
#define _CpmImpl_HPP_

#include <opencv2/core.hpp>

#include "CpmConfig.hpp"

class CpmImpl
{
public:
   static cv::Ptr<CpmImpl> create(CpmConfig::PmPropertyType type_);

public:
   CpmImpl();

public:
   void property_patch_match_impl(
         const std::vector<cv::Mat>& image1_,
         const std::vector<cv::Mat>& image2_,
         std::vector<cv::Mat>& flows_u_,
         std::vector<cv::Mat>& flows_v_,
         std::vector<cv::Mat>& flows_cost_,
         cv::Ptr<MatchCost> cost_func_ptr_,
         const std::vector<cv::Mat>& seeds_,
         const cv::Mat& seed_neighbors_,
         const CpmConfig& config_
   );

   void property_propagation(
         cv::Mat& property_,
         const cv::Mat& f_,
         const cv::Mat& g_,
         cv::Mat& cost_,
         cv::Ptr<MatchCost> cost_func_ptr_,
         int num_iterations_,
         float max_search_radius_,
         const cv::Mat& seed_coord_,
         const cv::Mat& neighbors_,
         int half_patch_size_,
         bool verbose_ = true,   // for debug purpose only
         int level_num_ = 0      // for debug purpose only
   );

   void
   property_init_from_coarser_level(
         const std::vector<cv::Mat>& image1_,
         const std::vector<cv::Mat>& image2_,
         const std::vector<cv::Mat>& seeds_,
         std::vector<cv::Mat>& seeds_property_,
         std::vector<cv::Mat>& flows_cost_,
         cv::Ptr<MatchCost> cost_func_ptr_,
         float pyramid_ratio_,
         int half_patch_size_,
         int coarse_level_num_,
         int fine_level_num_
   );

   virtual void property_random_init(
         cv::Mat& property_,
         const cv::Mat& seeds_,
         float max_property_value_
   ) = 0;

   virtual void property_to_uv(
         const cv::Mat& property_,
         const cv::Mat& seeds_,
         cv::Mat& u_,
         cv::Mat& v_
   ) = 0;

   virtual void property_to_uv(
         const float* p_property_,
         float& u_,
         float& v_,
         int x_,
         int y_
   ) = 0;

   virtual void random_search(
         const float* p_old_property_,
         float* p_try_property_,
         float delta_
   ) = 0;

   virtual void init_from_coarser_level(
         const float* p_coarse_level_property_,
         float* p_fine_level_property_,
         float scale_
   ) = 0;

public:
   void set_num_properties(int val_) {m_num_properties = val_;}
   int get_num_properties() const {return m_num_properties;}

   void set_min_search_value(float val_) {m_min_search_value = val_;}
   float get_min_search_value() const {return m_min_search_value;}

   void set_is_left_view(bool val_) {m_is_left_view = val_;}
   bool get_is_left_view() const {return m_is_left_view;}

private:
   int m_num_properties;

   float m_min_search_value; //!< value for exiting the random search
                           //!< for example, if it is 1, then when the search interval is less than 1, it
                           //!< stops random search

   bool m_is_left_view;
};

#endif //_CpmImpl_HPP_
