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
#include <opencv2/imgproc.hpp>

#include "MyTimer.hpp"

#include "MyImageProcessing.hpp"

#include "Cpm.hpp"
#include "CpmImpl.hpp"
#include "SiftDescriptor.hpp"

#include "pm_common.hpp"

#include "OpticalFlowKfj.hpp"

/**
 * Flow for occluded pixels
 */
static const int g_invalid_flow = 10000;

/**
 * Internal implementation for Coarse-to-fine PatchMatch (CPM).
 *
 * Since the neighbors of a seed do not change across levels,
 * seed_neighbors is not a vector of cv::Mat.
 *
 * @param image1_           [in] Image pyramid. The 0th element is the reference frame, which is the raw image.
 * @param image2_           [in] Image pyramid.
 * @param flows_u_          [out] Flow field for each level in the x-direction.
 * @param flows_v_          [out] Flow field for each level in the y-direction.
 * @param flows_cost_       [out] Flow matching cost for (flows_u_, flows_v_).
 * @param cost_func_ptr_    [in] Pointer to the function for computing matching cost.
 * @param seeds_            [in] Pixel coordinates of every seed in each level.
 * @param seed_neighbors_   [in] Neighbor indices of a seed.
 * @param config_           [in] CPM configuration.
 */
static void
patch_match_impl(
      const std::vector<cv::Mat>& image1_,
      const std::vector<cv::Mat>& image2_,
      std::vector<cv::Mat>& flows_u_,
      std::vector<cv::Mat>& flows_v_,
      std::vector<cv::Mat>& flows_cost_,
      cv::Ptr<MatchCost> cost_func_ptr_,
      const std::vector<cv::Mat>& seeds_,
      const cv::Mat& seed_neighbors_,
      const CpmConfig& config_,
      CpmConfig::ViewIndex view_index_
)
{
   auto ptr_cpm_impl = CpmImpl::create(config_.get_pm_property_type());
   CV_Assert(!ptr_cpm_impl.empty());
   ptr_cpm_impl->set_is_left_view(CpmConfig::ViewIndex::E_LEFT_VIEW == view_index_);
   ptr_cpm_impl->property_patch_match_impl(image1_, image2_, flows_u_, flows_v_, flows_cost_, cost_func_ptr_, seeds_, seed_neighbors_, config_);
}

Cpm::Cpm(
      const cv::Mat& image1_,
      const cv::Mat& image2_,
      const CpmConfig& config_,
      cv::Ptr<MatchCost> cost_ptr_
)
{
   init(image1_, image2_, config_, cost_ptr_);
}

void
Cpm::init(
      const cv::Mat& image1_,
      const cv::Mat& image2_,
      const CpmConfig& config_,
      cv::Ptr<MatchCost> cost_ptr_
)
{
   CV_Assert(!image1_.empty() && !image2_.empty());
   CV_Assert(image1_.size() == image2_.size());
   CV_Assert(image1_.type() == image2_.type());
   CV_Assert((image1_.depth() == CV_8U)  ||
             (image1_.depth() == CV_32F));

   if (image1_.depth() == CV_8U)
   {
      image1_.convertTo(m_f, CV_32F);
      image2_.convertTo(m_g, CV_32F);
   }
   else if (image1_.depth() == CV_32F)
   {
      m_f = image1_.clone();
      m_g = image2_.clone();
   }
   else
   {
      CV_Assert(false); // unreachable code
   }

   m_config = config_;
   m_cost_ptr = cost_ptr_;
}

void
Cpm::compute_optical_flow()
{
   MyTimer timer;
   cv::setRNGSeed(100); // make the result reproducible

   bool verbose = m_config.get_verbose();
   if (verbose)
   {
      m_config.show_parameters();
   }

   timer.start();
   init_pyramid();
   timer.stop();
   if (verbose)
   {
      std::cout << "init_pyramid took " << timer.get_s() << " s" << std::endl;
   }

   timer.start();
   compute_descriptor();
   timer.stop();
   if (verbose)
   {
      std::cout << "compute_descriptor took " << timer.get_s() << " s" << std::endl;
   }

   timer.start();
   init_seeds();
   timer.stop();
   if (verbose)
   {
      std::cout << "init_seeds took " << timer.get_s() << " s" << std::endl;
   }

   timer.start();
   run_patch_match();
   timer.stop();

   if (m_config.get_verbose())
   {
      std::cout << "run_patch_match took " << timer.get_s() << " s" << std::endl;
   }
}

void
Cpm::get_matches(cv::Mat& matches_)
{
   cv::Mat m;
   m.create(m_num_seeds, 4, CV_32FC1);

   int j = 0;
   for (int i = 0; i < m_num_seeds; i++)
   {
      int y1 = m_seeds[0].at<int>(i, 1);
      int x1 = m_seeds[0].at<int>(i, 0);

      float u = m_u.at<float>(y1,x1);
      float v = m_v.at<float>(y1,x1);
      if ((u >= g_invalid_flow) || (v >= g_invalid_flow))
      {
         // do not return invalid flows
         continue;
      }

      int y2 = cvRound(y1 + v);
      int x2 = cvRound(x1 + u);

      m.at<float>(j,0) = x1;
      m.at<float>(j,1) = y1;
      m.at<float>(j,2) = x2;
      m.at<float>(j,3) = y2;
      j++;
   }
   matches_ = m(cv::Range(0,j), cv::Range(0,4)).clone();
}

void
Cpm::init_pyramid()
{
   float ratio = m_config.get_pyramid_ratio();
   int num_levels = m_config.get_number_of_pyramid_levels();

   int minimum_width = m_config.get_minimum_image_width();

   if (num_levels < 1)
   {
      MyImageProcessing::get_image_pyramid_by_ratio(m_f, m_f_pyramid, ratio, minimum_width);
      num_levels = MyImageProcessing::get_image_pyramid_by_ratio(m_g, m_g_pyramid, ratio, minimum_width);
   }
   else
   {
      num_levels = MyImageProcessing::get_image_pyramid_by_levels(m_f, m_f_pyramid, ratio, num_levels, minimum_width);
      num_levels = MyImageProcessing::get_image_pyramid_by_levels(m_g, m_g_pyramid, ratio, num_levels, minimum_width);
   }

   if (num_levels != m_config.get_number_of_pyramid_levels())
   {
      m_config.set_number_of_pyramid_levels(num_levels);
   }

   if (fabs(ratio - m_config.get_pyramid_ratio()) < 1e-5)
   {
      m_config.set_pyramid_ratio(ratio);
   }
}

void
Cpm::compute_descriptor()
{
   CV_Assert(m_f_pyramid.size() > 0);
   CV_Assert(m_f_pyramid.size() == m_g_pyramid.size());

   int num_levels = m_config.get_number_of_pyramid_levels();

   m_f_pyramid_descriptor.resize((size_t)num_levels);
   m_g_pyramid_descriptor.resize((size_t)num_levels);

   DescriptorType desc_type = m_config.get_descriptor_type();
   switch (desc_type)
   {
      case DescriptorType::E_DESC_TYPE_SIFT:
         for (int i = 0; i < num_levels; i++)
         {
            SiftDescriptor::compute_sift_descriptor(m_f_pyramid[i], m_f_pyramid_descriptor[i]);
            SiftDescriptor::compute_sift_descriptor(m_g_pyramid[i], m_g_pyramid_descriptor[i]);
         }
         break;
      case DescriptorType::E_DESC_TYPE_RANK_TRANSFORM:
         // TODO: calculate the descriptor of the original image
         // TODO: and then compute the pyramid of the descriptor image !
         for (int i = 0; i < num_levels; i++)
         {
            MyImageProcessing::rank_transform(m_f_pyramid[i], m_f_pyramid_descriptor[i], 5, m_config.get_descriptor_color_to_gray()); // an odd number less than 16
            MyImageProcessing::rank_transform(m_g_pyramid[i], m_g_pyramid_descriptor[i], 5, m_config.get_descriptor_color_to_gray()); // an odd number less than 16
         }
         break;
      case DescriptorType::E_DESC_TYPE_CENSUS_TRANSFORM:
         // TODO: calculate the descriptor of the original image
         // TODO: and then compute the pyramid of the descriptor image !
         for (int i = 0; i < num_levels; i++)
         {
            MyImageProcessing::census_transform(m_f_pyramid[i], m_f_pyramid_descriptor[i], 5, m_config.get_descriptor_color_to_gray()); // 3 or 5
            MyImageProcessing::census_transform(m_g_pyramid[i], m_g_pyramid_descriptor[i], 5, m_config.get_descriptor_color_to_gray()); // 3 or 5
         }
         break;
      case DescriptorType::E_DESC_TYPE_COMPLETE_RANK_TRANSFORM:
         // TODO: calculate the descriptor of the original image
         // TODO: and then compute the pyramid of the descriptor image !
         for (int i = 0; i < num_levels; i++)
         {
            MyImageProcessing::complete_rank_transform(m_f_pyramid[i], m_f_pyramid_descriptor[i], 5, m_config.get_descriptor_color_to_gray()); // an odd number, less than 16
            MyImageProcessing::complete_rank_transform(m_g_pyramid[i], m_g_pyramid_descriptor[i], 5, m_config.get_descriptor_color_to_gray()); // an odd number, less than 16
         }
         break;
      case DescriptorType::E_DESC_TYPE_COMPLETE_CENSUS_TRANSFORM:
         // TODO: calculate the descriptor of the original image
         // TODO: and then compute the pyramid of the descriptor image !
         for (int i = 0; i < num_levels; i++)
         {
            MyImageProcessing::complete_census_transform(m_f_pyramid[i], m_f_pyramid_descriptor[i], 5, m_config.get_descriptor_color_to_gray()); // an odd number, less than 16
            MyImageProcessing::complete_census_transform(m_g_pyramid[i], m_g_pyramid_descriptor[i], 5, m_config.get_descriptor_color_to_gray()); // an odd number, less than 16
         }
         break;
      default:
         CV_Assert(false);  // unreachable code
         break;
   }
}

void
Cpm::init_seeds()
{
   CV_Assert(!m_f.empty());
   CV_Assert(m_f.size() == m_g.size());

   int num_levels = m_config.get_number_of_pyramid_levels();

   int step = m_config.get_grid_space();

   // init seeds for the raw image : m_seeds[0]
   m_seeds_per_row = m_f.cols / step;
   m_seeds_per_col = m_f.rows / step;
   m_seed_x_offset = step >> 1;
   m_seed_y_offset = step >> 1;
   m_num_seeds = m_seeds_per_col * m_seeds_per_row;

   m_seeds.resize((size_t)num_levels);

   m_seeds[0].create(m_num_seeds, 2, CV_32SC1); // column 0 - x, column 1 - y

   m_seed_neighbors.create(m_num_seeds, 8, CV_32SC1); // each seed has 8 neighbors

   m_seed_neighbors = -1; // seeds at corners and boundaries do not have enough neighbors,
                          // invalid neighbors are denoted by -1

   //
   // Neighbor indices
   // 5  1  4
   // 2  x  0
   // 6  3  7
   //
   int neighbor_offset[8][2] =
        { // (dy, dx)
             { 0,  1}, // right,       0
             {-1,  0}, // top,         1
             { 0, -1}, // left,        2
             { 1,  0}, // bottom,      3
             {-1,  1}, // top right,   4
             {-1, -1}, // top left,    5
             { 1, -1}, // bottom left, 6
             { 1,  1}, // bottom right,7
        };
   for (int i = 0; i < m_num_seeds; i++)
   {
      int* p = m_seeds[0].ptr<int>(i);
      int grid_x = i % m_seeds_per_row;
      int grid_y = i / m_seeds_per_row;

      int x_coord = grid_x*step + m_seed_x_offset;
      int y_coord = grid_y*step + m_seed_y_offset;

      p[0] = x_coord;
      p[1] = y_coord;

      // init neighbors
      for (int j = 0; j < 8; j++)
      {
         int n_y = grid_y + neighbor_offset[j][0];
         int n_x = grid_x + neighbor_offset[j][1];
         if (!is_inside(n_y, m_seeds_per_col) || !is_inside(n_x, m_seeds_per_row))
         {
            continue;
         }
         m_seed_neighbors.at<int>(i,j) = n_y * m_seeds_per_row + n_x;
      }
   }

   for (int i = 1; i < num_levels; i++)
   {
      m_seeds[i].create(m_num_seeds, 2, CV_32SC1);

      int nx = m_f_pyramid[i].cols;
      int ny = m_f_pyramid[i].rows;

      float ratio = (float)std::pow(m_config.get_pyramid_ratio(), i);
      for (int j = 0; j < m_num_seeds; j++)
      {
         int y = (int)(m_seeds[0].at<int>(j,1) * ratio);
         int x = (int)(m_seeds[0].at<int>(j,0) * ratio);

         y = cv::min(y, ny-1);
         x = cv::min(x, nx-1);

         m_seeds[i].at<int>(j,1) = y;
         m_seeds[i].at<int>(j,0) = x;
      }
   }
}

static void
cross_check(
      const cv::Mat& seeds_,
      const cv::Mat& u1_,
      const cv::Mat& v1_,
      const cv::Mat& u2_,
      const cv::Mat& v2_,
      cv::Mat& u,
      cv::Mat& v,
      int grid_space_,
      int grid_w_,
      float threshold_,
      float max_displacement_,
      bool verbose_
)
{
   u.create(u1_.size(), CV_32FC1);
   v.create(u1_.size(), CV_32FC1);

   u = 0;
   v = 0;

#ifdef KFJ_DEBUG
   cv::Mat my_u, my_v;
   u.copyTo(my_u);
   v.copyTo(my_v);
#endif

   int border_width = 5; // TODO: how to choose the border width ?

   int ny = u1_.rows;
   int nx = u1_.cols;

   int offset = grid_space_ / 2;

   int valid_num = 0;

   int num_seeds = seeds_.rows;
   for (int i = 0; i < num_seeds; i++)
   {
      int y = seeds_.at<int>(i, 1);
      int x = seeds_.at<int>(i, 0);

      float u1 = u1_.at<float>(y,x);
      float v1 = v1_.at<float>(y,x);
      if (cv::sqrt(u1*u1 + v1*v1) > max_displacement_)
      {
         u.at<float>(y,x) = g_invalid_flow;
         v.at<float>(y,x) = g_invalid_flow;

         continue;
      }

      int y2 = cvRound(y + v1);
      int x2 = cvRound(x + u1);

      int seed_y = (y2 - offset) / grid_space_; // Fixme: seed_y and seed_x may be out of range
      int seed_x = (x2 - offset) / grid_space_;

      int seed_index = seed_y * grid_w_ + seed_x;

      int other_y = seeds_.at<int>(seed_index, 1);
      int other_x = seeds_.at<int>(seed_index, 0);
      if ((other_x < border_width) || (other_x >= nx - border_width)
          || (other_y < border_width) || (other_y >= ny - border_width))
      {
         u.at<float>(y,x) = g_invalid_flow;
         v.at<float>(y,x) = g_invalid_flow;
         continue;
      }

      float u2 = u2_.at<float>(other_y, other_x);
      float v2 = v2_.at<float>(other_y, other_x);

      if (cv::sqrt(u2*u2 + v2*v2) > max_displacement_)
      {
         u.at<float>(y,x) = g_invalid_flow;
         v.at<float>(y,x) = g_invalid_flow;
         continue;
      }

      float diff = cv::sqrt((u1+u2)*(u1+u2) + (v1+v2)*(v1+v2));
      if (diff > threshold_)
      {
         u.at<float>(y,x) = g_invalid_flow;
         v.at<float>(y,x) = g_invalid_flow;
         continue;
      }

      u.at<float>(y,x) = u1;
      v.at<float>(y,x) = v1;
      valid_num++;

#ifdef KFJ_DEBUG
      my_u.at<float>(y,x) = u1;
      my_v.at<float>(y,x) = v1;
#endif
   }
   if (verbose_)
   {
      printf("There are %d matches\n", valid_num);
      printf("Percent %.3f%% \n", (float)valid_num/num_seeds*100);
   }

#ifdef KFJ_DEBUG
   cv::String my_filename = cv::format("after-cross-check.flo");
   OpticalFlowKfj of;
   of.set_estimated_flow(my_u, my_v);
   of.save_estimated_flow_to_file(my_filename.c_str());
   printf("Save flow after cross check to '%s'\n", my_filename.c_str());
#endif
}

void
Cpm::run_patch_match()
{
   CV_Assert(!m_f_pyramid_descriptor.empty());
   CV_Assert(m_f_pyramid_descriptor.size() == m_g_pyramid_descriptor.size());

   cv::Mat u, v;

   patch_match_impl(m_f_pyramid_descriptor, m_g_pyramid_descriptor, m_seeds_flow_u, m_seeds_flow_v,
                    m_seeds_flow_cost, m_cost_ptr, m_seeds, m_seed_neighbors, m_config, CpmConfig::ViewIndex::E_LEFT_VIEW);

   if (m_config.get_cross_check())
   {
      std::vector<cv::Mat> flows_u, flows_v, flows_cost;
      patch_match_impl(m_g_pyramid_descriptor, m_f_pyramid_descriptor, flows_u, flows_v,
                       flows_cost, m_cost_ptr, m_seeds, m_seed_neighbors, m_config, CpmConfig::ViewIndex::E_RIGHT_VIEW);

      cross_check(m_seeds[0], m_seeds_flow_u[0], m_seeds_flow_v[0], flows_u[0], flows_v[0],
                  u, v, m_config.get_grid_space(), m_seeds_per_row, 3, m_config.get_max_displacement(),
                  m_config.get_verbose());
   }
   else
   {
      u = m_seeds_flow_u[0];
      v = m_seeds_flow_v[0];
   }

   m_u.create(u.size(), CV_32FC1);
   m_v.create(v.size(), CV_32FC1);

   m_u = 0;
   m_v = 0;

   int num_seeds = m_seeds[0].rows;
   for (int i = 0; i < num_seeds; i++)
   {
      int y = m_seeds[0].at<int>(i, 1);
      int x = m_seeds[0].at<int>(i, 0);

      m_u.at<float>(y,x) = u.at<float>(y,x);
      m_v.at<float>(y,x) = v.at<float>(y,x);
   }
}
