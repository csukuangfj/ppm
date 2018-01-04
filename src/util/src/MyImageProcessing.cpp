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
#include <opencv2/ximgproc.hpp>

#include "MyImageProcessing.hpp"

#include "RankTransform.hpp"
#include "CensusTransform.hpp"
#include "CompleteRankTransform.hpp"
#include "CompleteCensusTransform.hpp"

#include "3rd-party/bicubic_interpolation.hpp"
#include "3rd-party/zoom.hpp"

#include "common.hpp"

static bool
is_inside(int x, int w)
{
   return (x >= 0) && (x < w);
}

static int
enforce_inside(int x, int w)
{
   int res = x;
   if(!is_inside(x, w))
   {
      if (x < 0) res = 0;
      else res = w - 1;
   }
   return res;
}

void
MyImageProcessing::gradient_x(const cv::Mat& image_, cv::Mat& grad_)
{
   cv::Mat kx, ky;
   cv::getDerivKernels(kx, ky, 1, 0, 3, true, CV_32F); // normalized is true
   cv::sepFilter2D(image_, grad_, CV_32F, kx, ky);
}

void
MyImageProcessing::gradient_y(const cv::Mat& image_, cv::Mat& grad_)
{
   cv::Mat kx, ky;
   cv::getDerivKernels(kx, ky, 0, 1, 3, true, CV_32F); // normalized is true
   cv::sepFilter2D(image_, grad_, CV_32F, kx, ky);
}

void
MyImageProcessing::gaussian_filtering(
      const cv::Mat& image_, cv::Mat& out_,
      float sigma_,
      float precision_ /* = 5*/
)
{
   CV_Assert(sigma_ > 0);
   int nx,ny;
   nx = ny = cvRound(precision_*sigma_*2 + 1) | 1; // or 1 -> odd number

   cv::Mat image;
   image_.convertTo(image, CV_32F);

   cv::GaussianBlur(image, out_,
                    cv::Size(nx, ny),
                    sigma_, sigma_);
}

int
MyImageProcessing::get_image_pyramid_by_ratio(
      const cv::Mat& raw_image_,
      std::vector<cv::Mat>& pyramids_,
      float& ratio_,
      int minimum_width_ /*=30*/
)
{
   static const float upper_ratio = 0.98f;
   CV_Assert(!raw_image_.empty()); // the raw image should be non-empty

   if ((ratio_ < 0.1) || (ratio_ > upper_ratio))
   {
      printf("Change ratio from %.2f to %.2f\n", ratio_, upper_ratio);
      ratio_ = upper_ratio;
   }

   if (minimum_width_ < 30)
   {
      minimum_width_ = 30;
   }

   int raw_width = raw_image_.cols;

   // width * (ratio)^num_levels = min_width, num_levels = log(min_width/width) / log(ratio);
   int num_levels = (int)(std::log((float)minimum_width_/raw_width) / std::log(ratio_));

   return get_image_pyramid_by_levels(raw_image_, pyramids_, ratio_, num_levels, minimum_width_);
}

int
MyImageProcessing::get_image_pyramid_by_levels(
      const cv::Mat& raw_image_,
      std::vector<cv::Mat>& pyramids_,
      float& ratio_,
      int num_levels_,
      int minimum_width_ /*=30*/
)
{
   static const float upper_ratio = 0.98f;
   CV_Assert(!raw_image_.empty()); // the raw image should be non-empty

   if ((ratio_ < 0.1) || (ratio_ > upper_ratio))
   {
      printf("Change ratio from %.2f to %.2f\n", ratio_, upper_ratio);
      ratio_ = upper_ratio;
   }

   if (minimum_width_ < 30) minimum_width_ = 30;

   if (num_levels_ < 1) num_levels_ = 1;

   // width * (ratio)^num_levels = min_width, num_levels = log(min_width/width) / log(ratio);
   int avail_levels = (int)(std::log((float)minimum_width_/raw_image_.cols) / std::log(ratio_));

   num_levels_ = cv::min(num_levels_, avail_levels);

   pyramids_.resize((size_t)num_levels_);
   pyramids_[0] = raw_image_.clone();
   gaussian_filtering(pyramids_[0], pyramids_[0], 0.8f, 5); // pre-smooth the finest scale

#if 0
   for (int i = 1; i < num_levels_; i++)
   {
      pyramids_[i] = downsample(pyramids_[i-1], ratio_);
   }

#else

   float base_sigma_ = (1/ratio_ - 1);
   int n = (int)(std::log(0.25) / std::log(ratio_));
   float n_sigma_ = base_sigma_ * n;

   for (int i = 1; i < num_levels_; i++)
   {
      cv::Mat tmp;

      int w = (int)(ratio_ * pyramids_[i-1].cols);
      int h = (int)(ratio_ * pyramids_[i-1].rows);

      if (i <= n)
      {
         float sig = base_sigma_ * i;
         gaussian_filtering(raw_image_, tmp, sig, 3);

         // bilinear interpolation
         cv::resize(raw_image_, pyramids_[i], cv::Size(w, h), 0, 0, cv::INTER_LINEAR);
      }
      else
      {
         gaussian_filtering(pyramids_[i-n], tmp, n_sigma_, 3);

         cv::resize(tmp, pyramids_[i], cv::Size(w, h), 0, 0, cv::INTER_LINEAR);
      }
   }
#endif

   return num_levels_;
}

void
MyImageProcessing::compute_edges(
      const cv::Mat &f_,
      cv::Mat &edges_,
      bool thinned_ /* = false*/
)
{
   CV_Assert(f_.type() == CV_32FC3);

   // TODO: set the model filename as a global static variable
   cv::String model_filename;

   //model_filename = cv::String(KFJ_DATA_PATH) + "/sed_model/modelFinal.yml.gz";
   model_filename = cv::String(KFJ_DATA_PATH) + "/sed_model/model.yml.gz";

   //model_filename = cv::String(KFJ_DATA_PATH) + "/sed_model/modelFinal.bin"; // must faster, but requires my patch

   cv::Mat image;
   f_.convertTo(image, CV_32F, 1.0f/255);

   edges_.create(image.size(), image.type());

   // TODO: change it to a static variable to avoid load the model repeatedly.
   //cv::Ptr<cv::ximgproc::StructuredEdgeDetection> pDollar = cv::ximgproc::StructuredEdgeDetection::create(model_filename); // need my patch
   cv::Ptr<cv::ximgproc::StructuredEdgeDetection> pDollar = cv::ximgproc::createStructuredEdgeDetection(model_filename); // need my patch

   pDollar->detectEdges(image, edges_);

   // TODO: compare the performance of thinned and un-thinned edges

   // TODO: use other edge detectors, such as Canny

   if (thinned_)
   {
      cv::Mat orientation_map;
      pDollar->computeOrientation(edges_, orientation_map);

      // edge_nms is thinner than edges
      cv::Mat edge_nms;
      pDollar->edgesNms(edges_, orientation_map, edge_nms, 2, 0, 1, true);

      edges_ = edge_nms;
   }
}

cv::Mat
MyImageProcessing::bicubic_interpolation(
      const cv::Mat &input_,
      float x_,
      float y_
)
{
   CV_Assert(input_.depth() == CV_32F);

   std::vector<cv::Mat> mv;
   mv.resize((size_t)input_.channels());
   cv::split(input_, mv.data());

   std::vector<float> val;
   val.reserve(mv.size());

   for (const auto& m : mv)
   {
      float t;
      t = ::bicubic_interpolation(m.ptr<float>(0),
                                  x_, y_,
                                  m.cols, m.rows);
      val.push_back(t);
   }

   cv::Mat res(1, 1, CV_32FC(input_.channels()), val.data());
   return res.clone();
}

cv::Mat
MyImageProcessing::downsample(
      const cv::Mat &input_,
      float factor_
)
{
   std::cout << "Downsample: factor " << factor_ << std::endl
         << "Image size: " << input_.size();
   CV_Assert(input_.depth() == CV_32F);
   CV_Assert(factor_ < 1.0f);
   CV_Assert(factor_ > 0);

   std::vector<cv::Mat> mv;
   mv.resize((size_t)input_.channels());
   cv::split(input_, mv.data());

   std::vector<cv::Mat> res_mv;
   res_mv.resize(mv.size());

   int num_channels = input_.channels();
   int ny = input_.rows;
   int nx = input_.cols;

   int nxx, nyy;
   zoom_size(nx, ny, nxx, nyy, factor_);

   for (int i = 0; i < num_channels; i++)
   {
      res_mv[i].create(nyy, nxx, CV_32FC1);
      zoom_out(reinterpret_cast<float*>(mv[i].data),
               reinterpret_cast<float*>(res_mv[i].data),
               nx,
               ny,
               factor_);
   }

   cv::Mat res;
   cv::merge(res_mv, res);
   return res;
}

cv::Mat
MyImageProcessing::nearest_neighbor_interpolation(
      const cv::Mat &input_,
      float x_,
      float y_
)
{
   CV_Assert(input_.depth() == CV_32F);
   int ny = input_.rows;
   int nx = input_.cols;
   int num_channels = input_.channels();

   cv::Mat res(1, 1, CV_32FC(num_channels));
   int x = enforce_inside(int(x_+0.5), nx);
   int y = enforce_inside(int(y_+0.5), ny);
   memcpy(res.data, input_.ptr<float>(y,x), sizeof(float)*num_channels);
   return res;
}

cv::Mat
MyImageProcessing::bilinear_interpolation(
      const cv::Mat &input_,
      float x_,
      float y_
)
{
   CV_Assert(input_.depth() == CV_32F);
   int ny = input_.rows;
   int nx = input_.cols;
   int num_channels = input_.channels();

   cv::Mat res(1, 1, CV_32FC(num_channels));
   int x = enforce_inside((int)x_, nx);
   int y = enforce_inside((int)y_, ny);
   float dx = x_ - x;
   float dy = y_ - y;
   for (int i = 0; i < num_channels; i++)
   {
      /*  dx  1-dx
        p0--------p1
        |   |     |
     dy |   |(x,y)|
        |---.-----|
        |   |     |
   1-dy |   |     |
        p3--------p2
       */

      float p0 = input_.ptr<float>(y,x)[i];
      float p1 = input_.ptr<float>(y,enforce_inside(x+1, nx))[i];
      float p2 = input_.ptr<float>(enforce_inside(y+1, ny),enforce_inside(x+1, nx))[i];
      float p3 = input_.ptr<float>(enforce_inside(y+1, ny),x)[i];
      float p;
      p = p0*(1-dx)*(1-dy) +
          p1*   dx *(1-dy) +
          p2*   dx *   dy  +
          p3*(1-dx)*   dy;
      res.ptr<float>(0,0)[i] = p;
   }

   return res;
}

cv::Mat
MyImageProcessing::get_patch(
      const cv::Mat &input_,
      float x_,
      float y_,
      int size_,
      InterpolationMode mode_
)
{
   std::function<cv::Mat (const cv::Mat&, float, float)> my_interpolation_func;
   switch (mode_)
   {
      case InterpolationMode::E_INTERPOLATION_NN:
         my_interpolation_func = &MyImageProcessing::nearest_neighbor_interpolation;
         break;
      case InterpolationMode ::E_INTERPOLATION_BILINEAR:
         my_interpolation_func = &MyImageProcessing::bilinear_interpolation;
         break;
      case InterpolationMode::E_INTERPOLATION_BIBUIC:
         my_interpolation_func = &MyImageProcessing::bicubic_interpolation;
         break;
      default:
         CV_Assert(false); // unreachable code
         break;
   }

   CV_Assert(input_.depth() == CV_32F);

   int ny = 2*size_+1;
   int nx = 2*size_+1;
   int num_channels = input_.channels();

   cv::Mat res(ny, nx, input_.type());
#pragma omp parallel for
   for (int dy = -size_; dy <= size_; dy++)
   {
      for (int dx = -size_; dx <= size_; dx++)
      {
         float px = x_ + dx;
         float py = y_ + dy;
         cv::Mat t = my_interpolation_func(input_, px, py);
         memcpy(res.ptr<float>(size_+dy, size_+dx),
                t.data,
                sizeof(float)*num_channels);
      }
   }

   return res;
}

void
MyImageProcessing::rank_transform(
      const cv::Mat &in_image_,
      cv::Mat &rank_image_,
      int wnd_size_,
      bool color_to_gray_
)
{
   return ::rank_transform(in_image_, rank_image_, wnd_size_, color_to_gray_);
}

void
MyImageProcessing::census_transform(
      const cv::Mat &in_image_,
      cv::Mat &census_image_,
      int wnd_size_,
      bool color_to_gray_
)
{
   return ::census_transform(in_image_, census_image_, wnd_size_, color_to_gray_);
}

void
MyImageProcessing::complete_rank_transform(
      const cv::Mat &in_image_,
      cv::Mat &rank_image_,
      int wnd_size_,
      bool color_to_gray_
)
{
   return ::complete_rank_transform(in_image_, rank_image_, wnd_size_, color_to_gray_);
}

void
MyImageProcessing::complete_census_transform(
      const cv::Mat &in_image_,
      cv::Mat &rank_image_,
      int wnd_size_,
      bool color_to_gray_
)
{
   return ::complete_census_transform(in_image_, rank_image_, wnd_size_, color_to_gray_);
}
