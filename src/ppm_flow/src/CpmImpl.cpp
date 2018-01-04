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
#include <opencv2/core.hpp>

#include "CpmImpl.hpp"
#include "CpmImplFlow.hpp"
#include "CpmImplAffineModel.hpp"
#include "CpmImplQuadraticModel.hpp"
#include "CpmImplProjectivePlanar.hpp"
#include "CpmConfig.hpp"
#include "MatchCost.hpp"
#include "pm_common.hpp"

#include "OpticalFlowKfj.hpp"

#define KFJ_SAVE_DEBUG_INFO 0

cv::Ptr<CpmImpl>
CpmImpl::create(CpmConfig::PmPropertyType type_)
{
   cv::Ptr<CpmImpl> res;
   switch (type_)
   {
      case CpmConfig::PmPropertyType::E_PROPERTY_FLOW:
         res = cv::makePtr<CpmImplFlow>();
         break;
      case CpmConfig::PmPropertyType::E_PROPERTY_AFFINE_MODEL:
         res = cv::makePtr<CpmImplAffineModel>();
         break;
      case CpmConfig::PmPropertyType::E_PROPERTY_QUADRATIC_MODEL:
         res = cv::makePtr<CpmImplQuadraticModel>();
         break;
      case CpmConfig::PmPropertyType::E_PROPERTY_PROJECTIVE_PLANAR:
         res = cv::makePtr<CpmImplProjectivePlanar>();
         break;
      default:
         CV_Assert(false);  // unreachable code
         break;
   }
   return res;
}

CpmImpl::CpmImpl()
   :
   m_is_left_view(true)
{}

void
CpmImpl::property_patch_match_impl(
      const std::vector<cv::Mat> &image1_,
      const std::vector<cv::Mat> &image2_,
      std::vector<cv::Mat> &flows_u_,
      std::vector<cv::Mat> &flows_v_,
      std::vector<cv::Mat> &flows_cost_,
      cv::Ptr<MatchCost> cost_func_ptr_,
      const std::vector<cv::Mat> &seeds_,
      const cv::Mat &seed_neighbors_,
      const CpmConfig &config_
)
{
   CV_Assert(!image1_.empty() && !image2_.empty());
   CV_Assert(image1_.size() == image2_.size());

   int num_levels = (int)image1_.size();
   for (int i = 0; i < num_levels; i++)
   {
      CV_Assert(image1_[i].size() == image2_[i].size());
      CV_Assert(image1_[i].type() == image2_[i].type());
   }

   flows_u_.resize((size_t)num_levels);
   flows_v_.resize((size_t)num_levels);
   flows_cost_.resize((size_t)num_levels);

   std::vector<cv::Mat> seeds_property((size_t)num_levels);

   float search_radius = config_.get_max_displacement() * (float)std::pow(config_.get_pyramid_ratio(), num_levels-1);

   seeds_property[num_levels-1].create(image1_[num_levels-1].size(), CV_32FC(m_num_properties));
   property_random_init(seeds_property[num_levels-1], seeds_[num_levels-1], search_radius);
   property_to_uv(seeds_property[num_levels-1], seeds_[num_levels-1], flows_u_[num_levels-1], flows_v_[num_levels-1]);
   compute_cost(flows_u_[num_levels-1], flows_v_[num_levels-1], image1_[num_levels-1], image2_[num_levels-1], flows_cost_[num_levels-1],
                cost_func_ptr_,seeds_[num_levels-1],config_.get_half_patch_size());

   cv::String filename = cv::format("/tmp/kuangfn/random-init-flow-level-%d.flo", num_levels-1);
   OpticalFlowKfj of;

#if KFJ_SAVE_DEBUG_INFO
   if (m_is_left_view)
   {
      of.set_estimated_flow(flows_u_[num_levels-1], flows_v_[num_levels-1]);
      of.save_estimated_flow_to_file(filename.c_str());
      printf("Save the random init flow to '%s'\n", filename.c_str());
   }
#endif

   int num_iterations = config_.get_number_of_iterations();

   property_propagation(
         seeds_property[num_levels-1],
         image1_[num_levels-1],
         image2_[num_levels-1],
         flows_cost_[num_levels-1],
         cost_func_ptr_,
         num_iterations,
         search_radius,
         seeds_[num_levels-1],
         seed_neighbors_,
         config_.get_half_patch_size(),
         config_.get_verbose(),
         num_levels - 1);

   property_to_uv(seeds_property[num_levels-1], seeds_[num_levels-1], flows_u_[num_levels-1], flows_v_[num_levels-1]);

#if KFJ_SAVE_DEBUG_INFO
   if (m_is_left_view)
   {
      filename = cv::format("/tmp/kuangfn/flow-level-%d.flo", num_levels-1);
      of.set_estimated_flow(flows_u_[num_levels-1], flows_v_[num_levels-1]);
      of.save_estimated_flow_to_file(filename.c_str());
      printf("Save flow level at %d to '%s'\n",num_levels-1, filename.c_str());
   }
#endif

   CV_Assert(config_.get_pyramid_ratio() > 1e-3);
   float inverse_ratio = 1.0f/config_.get_pyramid_ratio();

   for (int i = num_levels - 2; i >= 0; i--)
   {
      property_init_from_coarser_level(
            image1_,
            image2_,
            seeds_,
            seeds_property,
            flows_cost_,
            cost_func_ptr_,
            config_.get_pyramid_ratio(),
            config_.get_half_patch_size(),
            i+1,
            i);

#if KFJ_SAVE_DEBUG_INFO
      if (m_is_left_view)
      {
         property_to_uv(seeds_property[i], seeds_[i], flows_u_[i], flows_v_[i]);
         filename = cv::format("/tmp/kuangfn/flow-level-%d-init.flo", i);
         of.set_estimated_flow(flows_u_[i], flows_v_[i]);
         of.save_estimated_flow_to_file(filename.c_str());
         printf("Save flow level at %d to '%s'\n", i, filename.c_str());
      }
#endif

      search_radius *= inverse_ratio;// increase the search radius

      property_propagation(
            seeds_property[i],
            image1_[i],
            image2_[i],
            flows_cost_[i],
            cost_func_ptr_,
            num_iterations,
            search_radius,
            seeds_[i],
            seed_neighbors_,
            config_.get_half_patch_size(),
            config_.get_verbose(),
            i);
      property_to_uv(seeds_property[i], seeds_[i], flows_u_[i], flows_v_[i]);

#if KFJ_SAVE_DEBUG_INFO
      if (m_is_left_view)
      {
         filename = cv::format("/tmp/kuangfn/flow-level-%d.flo", i);
         of.set_estimated_flow(flows_u_[i], flows_v_[i]);
         of.save_estimated_flow_to_file(filename.c_str());
         printf("Save flow level at %d to '%s'\n", i, filename.c_str());
      }
#endif
   }
}

void
CpmImpl::property_propagation(
      cv::Mat &property_,
      const cv::Mat &f_,
      const cv::Mat &g_,
      cv::Mat &cost_,
      cv::Ptr<MatchCost> cost_func_ptr_,
      int num_iterations_,
      float max_search_radius_,
      const cv::Mat &seed_coord_,
      const cv::Mat &neighbors_,
      int half_patch_size_,
      bool verbose_ /* = true */,
      int level_num_ /* = 0 */
)
{
   int num_seeds = seed_coord_.rows;
   CV_Assert(property_.size() == f_.size());
   CV_Assert(property_.size() == cost_.size());
   CV_Assert(property_.type() == CV_32FC(m_num_properties));
   CV_Assert(cost_.type() == CV_32FC1);

   // 1:top neighbor, 2:left neighbor, 4:top right neighbor, 5: top left neighbor
   // 0:right neighbor, 3:bottom neighbor, 6:bottom left neighbor, 7:bottom right neighbor
   //
   //  5 1 4             1         5    4
   //  2 X 0   = X  +  2   0   +
   //  6 3 7             3         6    7
   // forward propagation: neighbors with 1,2,4,5
   // backward propagation: neighbors with 0,3,6,7
   std::vector<std::vector<int> > neighbor_indices = {{1,2,4,5}, {0,3,6,7}};

   static const float stop_ratio = 0.02f; //TODO: increase the number may increase AAE but will definitely reduce running time

   //TODO: the authors use 0.05
   float last_update_percent = 0;
   bool is_improved;
   int num_improved;

   float* ptr_try_property = new float[m_num_properties];
   CV_Assert(ptr_try_property);

   for (int i = 0; i < num_iterations_; i++)
   {
      num_improved = 0;
      for (int n = 0; n < num_seeds; n++)
      {
         is_improved = false;
         int y = seed_coord_.at<int>(n,1);
         int x = seed_coord_.at<int>(n,0);

         float* p_property = property_.ptr<float>(y,x);
         float old_u, old_v;
         property_to_uv(p_property, old_u, old_v, x, y);

         float& old_cost = cost_.at<float>(y,x);

         std::vector<int>& neighbor_index = neighbor_indices[(i&1)]; // even i: forward propagation, odd i: backward propagation
         int nz = (int)neighbor_index.size();

         for (int k = 0; k < nz; k++)
         {
            int index = neighbors_.at<int>(n, neighbor_index[k]);
            if (index == -1) continue; // the seed is on the corners or at the boundaries

            int y2 = seed_coord_.at<int>(index, 1);
            int x2 = seed_coord_.at<int>(index, 0);

            float* p_try_property = property_.ptr<float>(y2,x2);
            float try_u, try_v;
            property_to_uv(p_try_property, try_u, try_v, x, y);

            if ((cv::abs(try_u - old_u) < 1e-5) && (cv::abs(try_v - old_v) < 1e-5))
            {
               continue;
            }

            is_improved = improve_cost(f_, g_, x, y, old_u, old_v, old_cost, try_u, try_v, cost_func_ptr_, half_patch_size_);
            if (is_improved)
            {
               memcpy(p_property, p_try_property, sizeof(float)*m_num_properties);
               num_improved++;
            }
         }

         // random search
         for (float delta = max_search_radius_; delta > m_min_search_value; delta /= 2)
         {
            random_search(p_property, ptr_try_property, delta);

            float try_u, try_v;
            property_to_uv(ptr_try_property, try_u, try_v, x, y);

            if ((cv::abs(try_u - old_u) < 1e-5) && (cv::abs(try_v - old_v) < 1e-5))
            {
               continue;
            }

            is_improved = improve_cost(f_, g_, x, y, old_u, old_v, old_cost, try_u, try_v, cost_func_ptr_, half_patch_size_);
            if (is_improved)
            {
               memcpy(p_property, ptr_try_property, sizeof(float)*m_num_properties);
               num_improved++;
            }
         }
      } // end for (int n = 0; n < num_seeds; n++)

      if (verbose_)
      {
         std::cout << "number of improvement: " << num_improved << std::endl;
      }

      float update_percent = (float)num_improved/num_seeds;
      if ((update_percent < stop_ratio) || cv::abs(update_percent - last_update_percent) < 0.02)
      {
         if (verbose_)
         {
            std::cout << "********************" << std::endl
                      << "Exit the propagation loop in advance!" << std::endl
                      << "number of improvement: " << num_improved << std::endl
                      << "update percent: " << update_percent << std::endl
                      << "Level number: " << level_num_ << std::endl
                      << "Number of loops executed: " << (i+1) << std::endl
                      << "++++++++++++++++++++" << std::endl;
         }
         break;
      }
      last_update_percent = update_percent;
   } // for (int i = 0; i < num_iterations_; i++)

   delete []ptr_try_property;
}

void
CpmImpl::property_init_from_coarser_level(
      const std::vector<cv::Mat> &image1_,
      const std::vector<cv::Mat> &image2_,
      const std::vector<cv::Mat> &seeds_,
      std::vector<cv::Mat> &seeds_property_,
      std::vector<cv::Mat> &flows_cost_,
      cv::Ptr<MatchCost> cost_func_ptr_,
      float pyramid_ratio_,
      int half_patch_size_,
      int coarse_level_num_,
      int fine_level_num_
)
{
   int num_levels = (int)image1_.size();

   CV_Assert((coarse_level_num_ > 0) && (coarse_level_num_ < num_levels));
   CV_Assert((fine_level_num_ >= 0) && (fine_level_num_ < num_levels));
   CV_Assert(coarse_level_num_ == (fine_level_num_ + 1));

   CV_Assert(seeds_property_.size() == (size_t)num_levels);
   CV_Assert(flows_cost_.size() == (size_t)num_levels);
   CV_Assert(seeds_property_[coarse_level_num_].type() == CV_32FC(m_num_properties));

   CV_Assert(pyramid_ratio_ > 1e-3);
   float inverse_ratio = 1.0f/pyramid_ratio_; // assume the ratio is not 0!
   int half_patch_size = half_patch_size_;

   int nx = image1_[fine_level_num_].cols;
   int ny = image1_[fine_level_num_].rows;

   seeds_property_[fine_level_num_].create(ny, nx, CV_32FC(m_num_properties));
   flows_cost_[fine_level_num_].create(ny, nx, CV_32FC1);

   seeds_property_[fine_level_num_] = 0;
   flows_cost_[fine_level_num_] = 1e10;

   int num_seeds = seeds_[0].rows;

   for (int n = 0; n < num_seeds; n++)
   {
      int y_upper = seeds_[coarse_level_num_].at<int>(n, 1);
      int x_upper = seeds_[coarse_level_num_].at<int>(n, 0);
      float* p_property_coarse_level = seeds_property_[coarse_level_num_].ptr<float>(y_upper, x_upper);

      int y = seeds_[fine_level_num_].at<int>(n,1);
      int x = seeds_[fine_level_num_].at<int>(n,0);
      float* p_property_fine_level = seeds_property_[fine_level_num_].ptr<float>(y,x);

      init_from_coarser_level(p_property_coarse_level, p_property_fine_level, inverse_ratio);

      float u,v;
      property_to_uv(p_property_fine_level,u, v, x, y);

#if 1
      int x2 = cvRound(x + u);
      int y2 = cvRound(y + v);

      if (!is_inside(x2, nx) || !is_inside(y2, ny)) continue;
      float c = (float)cost_func_ptr_->compute_cost(image1_[fine_level_num_],
                                                    image2_[fine_level_num_],
                                                    cv::Point(x,y),
                                                    cv::Point(x2, y2),
                                                    half_patch_size);

      flows_cost_[fine_level_num_].at<float>(y,x) = c;
#else
      if (half_patch_size)
      {
         int x2 = cvRound(x + u);
         int y2 = cvRound(y + v);

         // todo: use bilinear interpolation when half_patch_size is 0
         if (!is_inside(x2, nx) || !is_inside(y2, ny)) continue;
         float c = (float)cost_func_ptr_->compute_cost(image1_[fine_level_num_],
                                                       image2_[fine_level_num_],
                                                       cv::Point(x,y),
                                                       cv::Point(x2, y2),
                                                       half_patch_size);

         flows_cost_[fine_level_num_].at<float>(y,x) = c;
      }
      else
      {
         // use bilinear interpolation when half_patch_size is 0
         // half patch size is 0,the cost is computed on the single pixel
         // assume that x is greater than 0
         /*
            (x_left, y_top)            (x_right, y_top)
               c1                              c2
               x--------------------------------x
               |            |                   |
             dy|            |                   |
               |------------x-------------------|
               |    dx      |      1-dx         |
           1-dy|            |                   |
               |            |                   |
               x--------------------------------x
               c3                               c4
       (x_left, y_bottom)              (x_right, y_bottom)
          */

         int x_left = (int)(x + u);
         int x_right = x_left + 1;
         float dx = x + u - x_left;

         int y_top = (int)(y+v);
         int y_bottom = y_top + 1;
         float dy = y + v - y_top;
         float c1 = 0, c2 = 0, c3 = 0, c4 = 0;

         bool updated = false;
         if (is_inside(x_left, nx) && is_inside(y_top, ny))
         {
            c1 = (float) cost_func_ptr_->compute_cost(image1_[fine_level_num_],
                                                      image2_[fine_level_num_],
                                                      cv::Point(x, y),
                                                      cv::Point(x_left, y_top),
                                                      half_patch_size_);
            updated = true;
         }

         if (is_inside(x_right, nx) && is_inside(y_top, ny))
         {
            c2 = (float) cost_func_ptr_->compute_cost(image1_[fine_level_num_],
                                                      image2_[fine_level_num_],
                                                      cv::Point(x, y),
                                                      cv::Point(x_right, y_top),
                                                      half_patch_size_);
            updated = true;
         }

         if (is_inside(x_left, nx) && is_inside(y_bottom, ny))
         {
            c3 = (float) cost_func_ptr_->compute_cost(image1_[fine_level_num_],
                                                      image2_[fine_level_num_],
                                                      cv::Point(x, y),
                                                      cv::Point(x_left, y_bottom),
                                                      half_patch_size_);
            updated = true;
         }

         if (is_inside(x_right, nx) && is_inside(y_bottom, ny))
         {
            c4 = (float) cost_func_ptr_->compute_cost(image1_[fine_level_num_],
                                                      image2_[fine_level_num_],
                                                      cv::Point(x, y),
                                                      cv::Point(x_right, y_bottom),
                                                      half_patch_size_);
            updated = true;
         }

         if (updated)
         {
            float c;
            c = c1*(1-dx)*(1-dy) +
                c2*   dx *(1-dy) +
                c3*(1-dx)*   dy  +
                c4*   dx *   dy;

            flows_cost_[fine_level_num_].at<float>(y,x) = c;
         }
      }
#endif
   }
}
