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
#include <opencv2/core.hpp>

#include "EpicFlowWrapper.hpp"
#include "image.h"
#include "array_types.h"

#include "epic.h"
#include "variational.h"

static cv::Mat
image_t_to_cv_mat(const image_t *in_image_)
{
   CV_Assert(in_image_);

   cv::Mat out_image;

   int ny = in_image_->height;
   int nx = in_image_->width;

   out_image.create(ny, nx, CV_32FC1);

   for (int y = 0; y < ny; y++)
   {
      int s = in_image_->stride*y;
      for (int x = 0; x < nx; x++)
      {
         int index = s + x;
         out_image.at<float>(y,x) = in_image_->data[index];
      }
   }

   return out_image;
}

/**
 * The user is responsible to free the memory using
 *  color_image_delete and to delete the returned pointer.
 *
 * @param in_image_ [in] CV_32FC3
 * @return
 */
static color_image_t*
cv_mat_to_color_image_t(const cv::Mat& in_image_)
{
   CV_Assert(in_image_.type() == CV_32FC3);

   color_image_t *out_image;

   int ny = in_image_.rows;
   int nx = in_image_.cols;

   out_image = color_image_new(nx, ny);

   for (int y = 0; y < ny; y++)
   {
      int s = out_image->stride*y;
      for (int x = 0; x < nx; x++)
      {
         const cv::Vec3f& pixel = in_image_.at<cv::Vec3f>(y,x);
         int index = s + x;
         out_image->c1[index] = pixel[2]; // r-channel
         out_image->c2[index] = pixel[1]; // g-channel
         out_image->c3[index] = pixel[0]; // b-channel
      }
   }
   return out_image;
}

/**
 * This function is equivalent to read_edges.
 *
 * The user is responsible to free the memory using
 *   free(out_image.pixels);
 *
 * @param in_image_ [in] CV_32FC1
 * @return
 */
static float_image
cv_mat_to_edges(const cv::Mat& in_image_)
{
   CV_Assert(in_image_.type() == CV_32FC1);

   int ny = in_image_.rows;
   int nx = in_image_.cols;
   float_image out_image  = empty_image(float, nx, ny);

   memcpy(out_image.pixels, in_image_.data, sizeof(float)*nx*ny);

   return out_image;
}

/**
 * This function is equivalent to read_matches
 * The user is responsible to free the memory using
 *  free(out_image.pixels);
 *
 * @param in_image_ [in] CV_32FC1 with 4 columns.
 *                       Column 0 is x1, column 1 is y1,
 *                       column 2 is x2, column 3 is y2
 * @return
 */
static float_image
cv_mat_to_matches(const cv::Mat& in_image_)
{
   CV_Assert(in_image_.type() == CV_32FC1);
   CV_Assert(in_image_.cols == 4);

   int ny = in_image_.rows;

   float_image out_image = empty_image(float, 4, ny);
   for (int y = 0; y < ny; y++)
   {
      out_image.pixels[4*y + 0] = in_image_.at<float>(y,0); // x1
      out_image.pixels[4*y + 1] = in_image_.at<float>(y,1); // y1
      out_image.pixels[4*y + 2] = in_image_.at<float>(y,2); // x2
      out_image.pixels[4*y + 3] = in_image_.at<float>(y,3); // y2
   }

   return out_image;
}

void
epic_flow_wrapper(
      color_image_t *im1_,
      color_image_t *im2_,
      float_image edges_,
      float_image matches_,
      epic_params_t epic_params_,
      variational_params_t flow_params_,
      image_t *wx_,
      image_t *wy_
)
{
   // compute interpolation and energy minimization
   color_image_t *imlab = rgb_to_lab(im1_);
   epic(wx_, wy_, imlab, &matches_, &edges_, &epic_params_, 6);

   // energy minimization
   variational(wx_, wy_, im1_, im2_, &flow_params_);

   color_image_delete(imlab);
}

void
EpicFlowWrapper::call_epic_flow()
{
   int ny = m_f.rows;
   int nx = m_f.cols;

   color_image_t *im1;
   color_image_t *im2;

   im1 = cv_mat_to_color_image_t(m_f);
   im2 = cv_mat_to_color_image_t(m_g);

   float_image edges;
   float_image matches;

   edges = cv_mat_to_edges(m_edges);
   matches = cv_mat_to_matches(m_matches);

   image_t *wx;
   image_t *wy;
   wx = image_new(nx, ny);
   wy = image_new(nx, ny);

   epic_params_t epic_params;
   variational_params_t flow_params;

   m_config.to_epic_params(epic_params);
   m_config.to_variational_params(flow_params);

   epic_flow_wrapper(im1, im2, edges, matches, epic_params, flow_params, wx, wy);

   m_u = image_t_to_cv_mat(wx);
   m_v = image_t_to_cv_mat(wy);

   free(edges.pixels);
   free(matches.pixels);

   image_delete(wx);
   image_delete(wy);

   color_image_delete(im1);
   color_image_delete(im2);
}

EpicFlowWrapper::EpicFlowWrapper(
      const cv::Mat& f_,
      const cv::Mat& g_,
      const cv::Mat& matches_,
      const cv::Mat& edges_,
      const EpicFlowConfig& config_
)
{
   init(f_, g_, matches_, edges_, config_);
}

void
EpicFlowWrapper::init(
      const cv::Mat& f_,
      const cv::Mat& g_,
      const cv::Mat& matches_,
      const cv::Mat& edges_,
      const EpicFlowConfig& config_
)
{
   CV_Assert(f_.type() == g_.type());
   CV_Assert(f_.type() == CV_32FC3);
   CV_Assert(edges_.type() == CV_32FC1);
   CV_Assert(matches_.type() == CV_32FC1);

   CV_Assert(f_.size() == g_.size());
   CV_Assert(f_.size() == edges_.size());

   CV_Assert(matches_.cols == 4);

#if 0
   m_f = f_.clone();
   m_g = g_.clone();
   m_matches = matches_.clone();
   m_edges = edges_.clone();
#else
   m_f = f_;
   m_g = g_;
   m_matches = matches_;
   m_edges = edges_;
#endif

   m_config = config_;
}
