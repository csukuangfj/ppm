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
#ifndef _PATCHMATCHSTEREOSLANTED_HPP_
#define _PATCHMATCHSTEREOSLANTED_HPP_

#include <opencv2/core.hpp>

#include "PatchMatchStereoSlantedConfig.hpp"
#include "PatchMatchStereoImpl.hpp"


/**
PatchMatchStereo with slanted windows
 Refer to the following paper:

 Bleyer, Michael, Christoph Rhemann, and Carsten Rother.
 "PatchMatch Stereo-Stereo Matching with Slanted Support Windows."
 Bmvc. Vol. 11. 2011.
 */
class PatchMatchStereoSlanted
{
public:
   PatchMatchStereoSlanted() {}

   PatchMatchStereoSlanted(
         const cv::Mat& left_view_,
         const cv::Mat& right_view_,
         const PatchMatchStereoSlantedConfig& config_
   );

   ~PatchMatchStereoSlanted();

   void init(
         const cv::Mat& left_view_,
         const cv::Mat& right_view_,
         const PatchMatchStereoSlantedConfig& config_
   );

   enum ViewIndex
   {
      LEFT_VIEW = 0,
      RIGHT_VIEW = 1,
      NUM_VIEWS = 2,
   };

   enum PropagationType
   {
      FORWARD_PROPAGATION,
      BACKWARD_PROPAGATION,
   };

   cv::Mat get_estimated_disparity_scaled(ViewIndex v) const
   {return m_estimated_disparity[v] * m_config.get_disparity_scale();}

   cv::Mat get_estimated_disparity_not_scaled(ViewIndex v) const
   {return m_estimated_disparity[v];}

   const cv::Mat& get_valid(ViewIndex v) const
   {return m_valid[v];}

   void run_PatchMatch_stereo();

private:
   void random_initialization();
   void propagation(PropagationType type_);

private:

   float compute_property_cost(
         const float* p_property_,
         int x_,
         int y_,
         ViewIndex v_
   );

   void improve_cost(
         int x_,
         int y_,
         float& old_cost_,
         float* p_old_property,
         const float* p_try_property,
         ViewIndex v_
   );

   void generate_disparity_map_not_scaled();

   void post_processing();
   void occlusion_check();
   void fill_occluded_pixels();
   void weighted_median_filter();
private:
   void compute_gradient_x();
   void compute_wpq_exp_lut();
   float compute_color_weight(const cv::Vec3f& p, const cv::Vec3f& q);

private:
   void allocate_color_dissimilarity_memory();
   void free_color_dissimilarity_memory();
   void precompute_color_dissimilarity();
private:
   cv::Mat m_views[NUM_VIEWS]; //!< CV_32FC3
   cv::Mat m_estimated_disparity[NUM_VIEWS]; //!< CV_32FC1
   cv::Mat m_valid[NUM_VIEWS]; //!< CV_8UC1, 255-valid, 0-invalid

private:
   PatchMatchStereoSlantedConfig m_config;

   cv::Mat m_cost[NUM_VIEWS]; //!< CV_32FC1

   cv::Mat m_grad_x[NUM_VIEWS]; //!< CV_32FC1

   float m_wpq_exp_lut[3*255+1];

   float ***m_dissimilarity[NUM_VIEWS]; //! (d,y,x), d is in [0, max_disparity]

   float m_bad_disparity_cost;

   cv::Ptr<PatchMatchStereoImpl> m_ptr_pmst_impl;
   cv::Mat m_properties[NUM_VIEWS];

   cv::String m_suffix; //!< for debug purpose
};

#endif //_PATCHMATCHSTEREOSLANTED_HPP_
