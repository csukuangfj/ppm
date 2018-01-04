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
#include <opencv2/imgproc.hpp>
#include <opencv2/imgcodecs.hpp>
#include "PatchMatchStereoSlanted.hpp"
#include "MyTimer.hpp"

static inline bool
is_inside(int x, int ncols)
{
   return (x >= 0) && (x < ncols);
}

static inline int
clip_to_border(int x, int ncols)
{
   int res = cv::max(0, cv::min(x, ncols-1));
   return res;
}

PatchMatchStereoSlanted::PatchMatchStereoSlanted(
      const cv::Mat& left_view_,
      const cv::Mat& right_view_,
      const PatchMatchStereoSlantedConfig& config_
)
{
   init(left_view_, right_view_, config_);
}

PatchMatchStereoSlanted::~PatchMatchStereoSlanted()
{
   free_color_dissimilarity_memory();
}

void
PatchMatchStereoSlanted::init(
      const cv::Mat& left_view_,
      const cv::Mat& right_view_,
      const PatchMatchStereoSlantedConfig& config_
)
{
   CV_Assert(left_view_.size() == right_view_.size());
   CV_Assert(left_view_.type() == right_view_.type());
   CV_Assert(left_view_.type() == CV_32FC3);

   m_views[LEFT_VIEW] = left_view_.clone();
   m_views[RIGHT_VIEW] = right_view_.clone();

   m_config = config_;

   m_dissimilarity[LEFT_VIEW] = m_dissimilarity[RIGHT_VIEW] = NULL;

   m_ptr_pmst_impl = PatchMatchStereoImpl::create(m_config.get_property_type());

   m_suffix = cv::format("-%d", (int)m_config.get_property_type());


   int num_properties = m_ptr_pmst_impl->get_num_properties();
   m_properties[LEFT_VIEW].create(left_view_.size(), CV_32FC(num_properties));
   m_properties[RIGHT_VIEW].create(left_view_.size(), CV_32FC(num_properties));
}

void
PatchMatchStereoSlanted::run_PatchMatch_stereo()
{
   const int s = 3;
   float alpha = m_config.get_alpha();
   float tau_color = m_config.get_tau_color();
   float tau_grad = m_config.get_tau_gradient();
   m_bad_disparity_cost = s * ((1-alpha)*tau_color + alpha*tau_grad);

   cv::String output_directory = m_config.get_output_directory();

   bool verbose = m_config.get_verbose();
   MyTimer timer_total;
   timer_total.start();
   if (verbose) printf("***** started *****\n");

   MyTimer timer;

   timer.start();
   if (verbose) printf("compute gradient started...\n");

   // pre-compute the gradient information
   compute_gradient_x();

   timer.stop();
   if (verbose) printf("finished in: %.4f ms, or %.4f s\n\n", timer.get_ms(), timer.get_s());

   timer.start();
   if (verbose) printf("precompute dissimilarity cost...\n");

   precompute_color_dissimilarity();

   timer.stop();
   if (verbose) printf("finished in: %.4f ms, or %.4f s\n\n", timer.get_ms(), timer.get_s());

   timer.start();
   if (verbose) printf("compute exp look up table...\n");

   compute_wpq_exp_lut();

   timer.stop();
   if (verbose) printf("finished in: %.4f ms, or %.4f s\n\n", timer.get_ms(), timer.get_s());

   timer.start();
   if (verbose) printf("random initialization started\n");

   random_initialization();

// debug output - start
   generate_disparity_map_not_scaled();
   char ss[100];
   snprintf(ss, sizeof(ss), "%s/random-left%s.png", output_directory.c_str(), m_suffix.c_str());
   cv::Mat tmp;
   get_estimated_disparity_scaled(LEFT_VIEW).convertTo(tmp, CV_8UC1);
   cv::imwrite(ss, tmp);

   snprintf(ss, sizeof(ss), "%s/random-right%s.png", output_directory.c_str(), m_suffix.c_str());
   get_estimated_disparity_scaled(RIGHT_VIEW).convertTo(tmp, CV_8UC1);
   cv::imwrite(ss, tmp);
// debug output - end

   timer.stop();
   if (verbose) printf("finished in: %.4f ms, or %.4f s\n\n", timer.get_ms(), timer.get_s());

   if (verbose) printf("iterations started\n");
   double total_time = 0;
   int num_iter = m_config.get_number_of_iterations();

   for (int i = 0; i < num_iter; i++)
   {
      if (verbose) printf("iteration %d started\n", i);
      timer.start();

      if ((i & 1) == 0)
      {
         propagation(FORWARD_PROPAGATION);
      }
      else
      {
         propagation(BACKWARD_PROPAGATION);
      }

      generate_disparity_map_not_scaled();

      char s[100];
      snprintf(s, sizeof(s), "%s/iterations-%d-left%s.png", output_directory.c_str(), i, m_suffix.c_str());
      cv::Mat tmp;
      get_estimated_disparity_scaled(LEFT_VIEW).convertTo(tmp, CV_8UC1);
      cv::imwrite(s, tmp);

      snprintf(s, sizeof(s), "%s/iterations-%d-right%s.png", output_directory.c_str(), i, m_suffix.c_str());
      get_estimated_disparity_scaled(RIGHT_VIEW).convertTo(tmp, CV_8UC1);
      cv::imwrite(s, tmp);

      timer.stop();
      if (verbose) printf("finished in: %.4f ms, or %.4f s\n", timer.get_ms(), timer.get_s());
      total_time += timer.get_ms();
   }
   if (verbose) printf("iteration finished in: %.4f ms, or %.4f s\n\n", total_time, total_time/1000);

   timer.start();
   if (verbose) printf("post processing started\n");

   post_processing();

   timer.stop();
   if (verbose) printf("finished in: %.4f ms, or %.4f s\n\n", timer.get_ms(), timer.get_s());

   timer_total.stop();
   if (verbose) printf("***** finished *****\n");
   if (verbose) printf("It took %.3f seconds, i.e, %.3f minutes\n", timer_total.get_s(), timer_total.get_s()/60.0f);

   cv::imwrite(output_directory + "/final-left" + m_suffix +".png", get_estimated_disparity_scaled(LEFT_VIEW));
   cv::imwrite(output_directory + "/final-right" + m_suffix + ".png", get_estimated_disparity_scaled(RIGHT_VIEW));
}

void
PatchMatchStereoSlanted::random_initialization()
{
   cv::setRNGSeed(100); // make the result reproducible
   //cv::setRNGSeed((int)cv::getTickCount());

   int nx = m_views[LEFT_VIEW].cols;
   int ny = m_views[LEFT_VIEW].rows;

   m_cost[LEFT_VIEW].create(ny, nx, CV_32FC1);
   m_cost[RIGHT_VIEW].create(ny, nx, CV_32FC1);

   float max_disparity = m_config.get_max_disparity();

   for (int v = LEFT_VIEW; v < NUM_VIEWS; v++)
   {
      for (int y = 0; y < ny; y++)
      {
         float* p_cost = m_cost[v].ptr<float>(y);
         for (int x = 0; x < nx; x++)
         {
            float* p_property = m_properties[v].ptr<float>(y,x);
            m_ptr_pmst_impl->property_random_init(p_property, max_disparity, x, y);
            p_cost[x] = compute_property_cost(p_property, x, y, (ViewIndex)v);
         }
      }
   }
}

void
PatchMatchStereoSlanted::propagation(PropagationType type_)
{
   cv::String output_directory = m_config.get_output_directory(); // debug

   int nx = m_views[LEFT_VIEW].cols;
   int ny = m_views[LEFT_VIEW].rows;

   int ystart = 0, yend = ny, ychange = +1;
   int xstart = 0, xend = nx, xchange = +1;
   if (BACKWARD_PROPAGATION == type_)
   {
      ystart = ny - 1; yend = -1; ychange = -1;
      xstart = nx - 1; xend = -1; xchange = -1;
   }

   int sign;
   int max_disparity = m_config.get_max_disparity();

   bool verbose = m_config.get_verbose();

   int iter_step = 5; //  debug, 1/5, 2/5, 3/5, 4/5, 5/5 output

   int current_step_y = ny/5;  // debug,
   int counter = 0; // debug
   int iter_num = 0;

   for (int v = LEFT_VIEW; v < NUM_VIEWS; v++)
   {
      counter = 0;
      iter_num = 0;
      if (LEFT_VIEW == v) sign = 1;
      else sign = -1;
      for (int y = ystart; y != yend; y += ychange)
      {
         counter++;
         if ((counter % current_step_y) == 0)
         {
// debug output - start
            iter_num++;

            generate_disparity_map_not_scaled();
            char ss[100];
            snprintf(ss, sizeof(ss), "%s/iter---%d-%d---view-%d-model-%s.png", output_directory.c_str(), iter_num, iter_step, v, m_suffix.c_str());
            cv::Mat tmp;
            get_estimated_disparity_scaled((ViewIndex)v).convertTo(tmp, CV_8UC1);
            cv::imwrite(ss, tmp);
// debug output - end
         }

         if (verbose && ((y%50) == 0))
            printf("v: %d, row: %d\n", v, y);
         float* p_cost = m_cost[v].ptr<float>(y);

         float* p_cost_other = m_cost[1-v].ptr<float>(y);

         for (int x = xstart; x != xend; x += xchange)
         {
            float& best_cost = p_cost[x];

            float* p_old_property = m_properties[v].ptr<float>(y,x);

            // spatial propagation
            if (is_inside(x - xchange, nx))
            {
               const float* p_try_property = m_properties[v].ptr<float>(y,x-xchange);
               improve_cost(x, y, best_cost, p_old_property, p_try_property, (ViewIndex)v);
            }

            if (is_inside(y - ychange, ny))
            {
               const float* p_try_property = m_properties[v].ptr<float>(y-ychange,x);
               improve_cost(x, y, best_cost, p_old_property, p_try_property, (ViewIndex)v);
            }

            if (is_inside(x - xchange, nx) && is_inside(y - ychange, ny))
            {
               const float* p_try_property = m_properties[v].ptr<float>(y-ychange,x-xchange);
               improve_cost(x, y, best_cost, p_old_property, p_try_property, (ViewIndex)v);
            }

            if (is_inside(x + xchange, nx) && is_inside(y - ychange, ny))
            {
               const float* p_try_property = m_properties[v].ptr<float>(y-ychange,x+xchange);
               improve_cost(x, y, best_cost, p_old_property, p_try_property, (ViewIndex)v);
            }

            // view propagation
            float d_ = m_ptr_pmst_impl->compute_disparity(p_old_property, x, y, LEFT_VIEW == v);
            int d = cvRound(d_);
            if ( (d >= 0) && (d <= max_disparity))
            {
               int other_x = x - sign*d;
               other_x = clip_to_border(other_x, nx);

               float& other_cost_best = p_cost_other[other_x];

               //const float* p_try_property = m_properties[1-v].ptr<float>(y, other_x);
               float* p_try_property = new float[m_ptr_pmst_impl->get_num_properties()];
               m_ptr_pmst_impl->property_view_conversion(p_old_property, p_try_property, other_x, y, d_, v == LEFT_VIEW);

               float* p_other_old_property = m_properties[1-v].ptr<float>(y, other_x);
               improve_cost(other_x, y, other_cost_best, p_other_old_property, p_try_property, (ViewIndex)(1-v));
               delete[] p_try_property;
            }

            float delta = m_config.get_max_disparity();

            delta /= 2;

            float min_search_value = m_ptr_pmst_impl->get_min_search_value();

            float* p_try_property = new float[m_ptr_pmst_impl->get_num_properties()];
            int k = 1;
            while(delta > min_search_value)
            {
               m_ptr_pmst_impl->random_search(p_old_property, p_try_property, delta, x, y, k++, LEFT_VIEW == v);
               // todo: do not call improve_cost if p_old_property and p_try_property are nearly equal
               improve_cost(x, y, best_cost, p_old_property, p_try_property, (ViewIndex)v);

               delta /= 2;
            }
            delete[] p_try_property;
         }
      }
   }
}

void
PatchMatchStereoSlanted::post_processing()
{
   bool verbose = m_config.get_verbose();
   cv::String output_directory = m_config.get_output_directory();

   generate_disparity_map_not_scaled();

   MyTimer timer;

   timer.start();
   if (verbose) printf("occlusion check started...\n");

   occlusion_check();

   timer.stop();
   if (verbose) printf("finished in: %.4f ms, or %.4f s\n\n", timer.get_ms(), timer.get_s());

   cv::imwrite(output_directory + "/left-occluded" + m_suffix + ".png", m_valid[LEFT_VIEW]);
   cv::imwrite(output_directory + "/right-occluded" + m_suffix + ".png", m_valid[RIGHT_VIEW]);

   timer.start();
   if (verbose) printf("fill occluded pixels started...\n");

   fill_occluded_pixels();

   timer.stop();
   if (verbose) printf("finished in: %.4f ms, or %.4f s\n\n", timer.get_ms(), timer.get_s());

   cv::Mat tmp;
   get_estimated_disparity_scaled(LEFT_VIEW).convertTo(tmp, CV_8UC1);
   cv::imwrite(output_directory + "/left-filled" + m_suffix + ".png", tmp);
   get_estimated_disparity_scaled(RIGHT_VIEW).convertTo(tmp, CV_8UC1);
   cv::imwrite(output_directory + "/right-filled" + m_suffix + ".png", tmp);

   timer.start();
   if (verbose) printf("weighted median filter started...\n");

   weighted_median_filter();

   timer.stop();
   if (verbose) printf("finished in: %.4f ms, or %.4f s\n\n", timer.get_ms(), timer.get_s());

   if (verbose) printf("post processing done!\n");
}

void
PatchMatchStereoSlanted::occlusion_check()
{
   CV_Assert(m_estimated_disparity[LEFT_VIEW].total() > 0);
   CV_Assert(m_estimated_disparity[LEFT_VIEW].type() == CV_32FC1);

   int nx = m_views[LEFT_VIEW].cols;
   int ny = m_views[LEFT_VIEW].rows;

   for (int v = LEFT_VIEW; v < NUM_VIEWS; v++)
   {
      m_valid[v].create(ny, nx, CV_8UC1);
      m_valid[v] = cv::Scalar(0);

      int sign = 1;
      if (RIGHT_VIEW == v) sign = -1;

      for (int y = 0; y < ny; y++)
      {
         float* pd_this = m_estimated_disparity[v].ptr<float>(y);
         float* pd_other = m_estimated_disparity[1-v].ptr<float>(y);

         uchar* p_valid = m_valid[v].ptr<uchar>(y);

         for (int x = 0; x < nx; x++)
         {
            float this_disparity = pd_this[x];
            int other_x = cvRound(x - sign*this_disparity);
            if (is_inside(other_x, nx))
            {
               float other_disparity = pd_other[other_x];
               float diff = this_disparity - other_disparity;
               // todo: is the threshold appropriate ?
               if ((cv::abs(diff) < 0.5) && (this_disparity > 0) && (other_disparity > 0))
               {
                  p_valid[x] = 255;
               }
            }
         }
      }
   }
}

void
PatchMatchStereoSlanted::fill_occluded_pixels()
{
   CV_Assert(m_estimated_disparity[LEFT_VIEW].total() > 0);
   CV_Assert(m_estimated_disparity[LEFT_VIEW].type() == CV_32FC1);

   int nx = m_views[LEFT_VIEW].cols;
   int ny = m_views[LEFT_VIEW].rows;

   for (int v = LEFT_VIEW; v < NUM_VIEWS; v++)
   {
      for (int y = 0; y < ny; y++)
      {
         const uchar* p_valid = m_valid[v].ptr<uchar>(y);
         float* pd = m_estimated_disparity[v].ptr<float>(y);
         for (int x = 0; x < nx; x++)
         {
            if (p_valid[x]) continue;
            int left_x = x-1;
            int right_x = x+1;
            bool is_left_valid = false;
            bool is_right_valid = false;
            while(is_inside(left_x, nx))
            {
               if (p_valid[left_x])
               {
                  is_left_valid = true;
                  break;
               }
               left_x--;
            }

            while(is_inside(right_x, nx))
            {
               if (p_valid[right_x])
               {
                  is_right_valid = true;
                  break;
               }
               right_x++;
            }

            if (is_left_valid && is_right_valid)
            {
               float left_d = m_ptr_pmst_impl->compute_disparity(m_properties[v].ptr<float>(y,left_x), x, y, LEFT_VIEW == v);
               float right_d = m_ptr_pmst_impl->compute_disparity(m_properties[v].ptr<float>(y,right_x), x, y, LEFT_VIEW == v);

               if (left_d < right_d)
               {
                  m_ptr_pmst_impl->copy_properties(m_properties[v].ptr<float>(y,left_x), m_properties[v].ptr<float>(y,x), x, y, left_d);
                  pd[x] = left_d;
               }
               else
               {
                  m_ptr_pmst_impl->copy_properties(m_properties[v].ptr<float>(y,right_x), m_properties[v].ptr<float>(y,x), x, y, right_d);
                  pd[x] = right_d;
               }
            }
            else if (is_left_valid)
            {
               float left_d = m_ptr_pmst_impl->compute_disparity(m_properties[v].ptr<float>(y,left_x), x, y, LEFT_VIEW == v);
               m_ptr_pmst_impl->copy_properties(m_properties[v].ptr<float>(y,left_x), m_properties[v].ptr<float>(y,x), x, y, left_d);
               pd[x] = left_d;
            }
            else if (is_right_valid)
            {
               float right_d = m_ptr_pmst_impl->compute_disparity(m_properties[v].ptr<float>(y,right_x), x, y, LEFT_VIEW == v);
               m_ptr_pmst_impl->copy_properties(m_properties[v].ptr<float>(y,right_x), m_properties[v].ptr<float>(y,x), x, y, right_d);
               pd[x] = right_d;
            }
            else
            {
               // this row is invalid, do nothing
            }
         } // end for x
      } // end for y
   } // end for v
}

void
PatchMatchStereoSlanted::weighted_median_filter()
{
   int nx = m_views[LEFT_VIEW].cols;
   int ny = m_views[LEFT_VIEW].rows;

   int half_sz = m_config.get_half_patch_size();

   // <disparity, weight>
   std::vector<std::pair<float, float> > dw_pairs;
   for (int v = LEFT_VIEW; v < NUM_VIEWS; v++)
   {
      for (int y = 0; y < ny; y++)
      {
         const uchar* p_valid = m_valid[v].ptr<uchar>(y);
         for (int x = 0; x < nx; x++)
         {
            if (p_valid[x]) continue;

            // only processing occluded pixels
            dw_pairs.clear();
            float tsum = 0;

            const cv::Vec3f& I_p = m_views[v].at<cv::Vec3f>(y,x);

            for (int dy = -half_sz; dy <= half_sz; dy++)
            {
               int y_ = y + dy;
               if (!is_inside(y_, ny)) continue;
               const uchar* q_valid = m_valid[v].ptr<uchar>(y_);
               const float* p_disp_ = m_estimated_disparity[v].ptr<float>(y_);
               //const float* p_weight = weight[v][r][c].ptr<float>(dy+half_sz);
               for (int dx = -half_sz; dx <= half_sz; dx++)
               {
                  int x_ = x + dx;
                  if (!is_inside(x_, nx)) continue;
                  if (!q_valid[x_]) continue; // todo: to include invalid pixels that have been filled
                  //int disp_ = (int)(p_disp_[x] + 0.5f);
                  //if ((disp_ < 0) || (disp_ > conf.max_disparity)) continue;
                  const cv::Vec3f& I_q = m_views[v].at<cv::Vec3f>(y_, x_);
                  float w_ = compute_color_weight(I_p, I_q);

                  dw_pairs.push_back({p_disp_[x_], w_}); // fixed: it is p_disp_[x_] instead of p_disp_[x] !
                  tsum += w_;
               }
            }

            std::sort(dw_pairs.begin(), dw_pairs.end());
            float tw = 0.0f;
            float tmedian = tsum/2;
            for (size_t i = 0; i < dw_pairs.size(); i++)
            {
               tw += dw_pairs[i].second;
               if (tw >= tmedian)
               {
                  if (i > 0)
                  {
                     m_estimated_disparity[v].at<float>(y,x) = (dw_pairs[i-1].first + dw_pairs[i].first)/2;
                  }
                  else
                  {
                     m_estimated_disparity[v].at<float>(y,x) = dw_pairs[i].first;
                  }
                  break;
               }
            }

         }
      }
   }
}

float
PatchMatchStereoSlanted::compute_property_cost(
      const float* p_property_,
      int x_,
      int y_,
      ViewIndex v_
)
{
   float cost = 0;

   int nx = m_views[LEFT_VIEW].cols;
   int ny = m_views[LEFT_VIEW].rows;

   int max_disparity = m_config.get_max_disparity();

   const cv::Vec3f& p = m_views[v_].at<cv::Vec3f>(y_, x_);

   int half_patch_sz = m_config.get_half_patch_size();

#ifdef KFJ_USE_OPENMP
   #pragma omp parallel for num_threads(5) reduction (+:cost)
#endif
   for (int dy = -half_patch_sz; dy <= half_patch_sz; dy++)
   {
      int y = y_ + dy;
      if (!is_inside(y, ny)) continue;
      const cv::Vec3f* q = m_views[v_].ptr<cv::Vec3f>(y);
      for (int dx = -half_patch_sz; dx <= half_patch_sz; dx++)
      {
         int x = x_ + dx;
         if (!is_inside(x, nx)) continue;
         float w_pq = compute_color_weight(p, q[x]);

         float disparity = m_ptr_pmst_impl->compute_disparity(p_property_, x, y, LEFT_VIEW == v_);
         if ((disparity < 0) || (disparity > max_disparity))
         {
            cost += w_pq * m_bad_disparity_cost;
            continue;
         }

         int left_d = (int)disparity;
         int right_d = left_d;
         if (disparity > left_d)
         {
            right_d = left_d + 1;
         }
         float left_alpha = right_d - disparity;
         float right_alpha = 1 - left_alpha;

         float left_cost = m_dissimilarity[v_][left_d][y][x];
         float right_cost = m_dissimilarity[v_][right_d][y][x];

         float cost_dis;
         cost_dis = left_alpha*left_cost + right_alpha*right_cost;

         cost += w_pq*cost_dis;
      }
   }
   return cost;
}

void
PatchMatchStereoSlanted::improve_cost(
      int x_,
      int y_,
      float &old_cost_,
      float *p_old_property,
      const float *p_try_property,
      ViewIndex v_)
{
   float cost_ = compute_property_cost(p_try_property, x_, y_, v_);
   if (cost_ < old_cost_)
   {
      old_cost_ =  cost_;
      int num_properties = m_ptr_pmst_impl->get_num_properties();
      memcpy(p_old_property, p_try_property, sizeof(float)*num_properties);
   }
}

void
PatchMatchStereoSlanted::generate_disparity_map_not_scaled()
{
   int nx = m_views[LEFT_VIEW].cols;
   int ny = m_views[LEFT_VIEW].rows;

   m_estimated_disparity[LEFT_VIEW].create(ny, nx, CV_32FC1);
   m_estimated_disparity[RIGHT_VIEW].create(ny, nx, CV_32FC1);

   for (int v = LEFT_VIEW; v < NUM_VIEWS; v++)
   {
      for (int y = 0; y < ny; y++)
      {
         float* p_disparity = m_estimated_disparity[v].ptr<float>(y);
         for (int x = 0; x < nx; x++)
         {
            float* p_property = m_properties[v].ptr<float>(y,x);
            p_disparity[x] = m_ptr_pmst_impl->compute_disparity(p_property, x, y, LEFT_VIEW == v);
         }
      }
   }
}

void
PatchMatchStereoSlanted::compute_gradient_x()
{
   int nx = m_views[LEFT_VIEW].cols;
   int ny = m_views[LEFT_VIEW].rows;

   for (int v = LEFT_VIEW; v < NUM_VIEWS; v++)
   {
      m_grad_x[v].create(ny, nx, CV_32FC1);

      cv::Mat t, help;
      cv::cvtColor(m_views[v], help, cv::COLOR_BGR2GRAY);
      help.convertTo(t, CV_32FC1);

      for (int y = 0; y < ny; y++)
      {
         float* p_grad = m_grad_x[v].ptr<float>(y);
         const float* p_img = t.ptr<float>(y);

         p_grad[0] = p_img[1] - p_img[0];
         p_grad[nx-1] = p_img[nx-1] - p_img[nx-2];
         for (int x = 1; x < nx-1; x++)
         {
            p_grad[x] = (p_img[x+1] - p_img[x-1]) * 0.5f;
         }
      }
   }
}

void
PatchMatchStereoSlanted::compute_wpq_exp_lut()
{
   float gamma_color = m_config.get_gamma_color();
   int n = 255*3;
   for (int i = 0; i <= n; i++)
   {
      m_wpq_exp_lut[i] = cv::exp(-i/gamma_color);
   }
}

float
PatchMatchStereoSlanted::compute_color_weight(const cv::Vec3f &p, const cv::Vec3f &q)
{
   //int id = (int)cv::norm(p, q, cv::NORM_L1);
   int id = 0;
   id = (int)cv::abs(p[0] - q[0]);
   id += (int)cv::abs(p[1] - q[1]);
   id += (int)cv::abs(p[2] - q[2]);

   return m_wpq_exp_lut[id];
}

void
PatchMatchStereoSlanted::allocate_color_dissimilarity_memory()
{
   if (m_dissimilarity[LEFT_VIEW]) free_color_dissimilarity_memory();

   int nx = m_views[LEFT_VIEW].cols;
   int ny = m_views[LEFT_VIEW].rows;

   int max_disparity = m_config.get_max_disparity();

   for (int v = LEFT_VIEW; v < NUM_VIEWS; v++)
   {
      m_dissimilarity[v] = new float**[max_disparity+1];
      CV_Assert(m_dissimilarity[v]);
      for (int d = 0; d <= max_disparity; d++)
      {
         m_dissimilarity[v][d] = new float*[ny];
         CV_Assert(m_dissimilarity[v][d]);
         for (int y = 0; y < ny; y++)
         {
            m_dissimilarity[v][d][y] = new float[nx];
            CV_Assert(m_dissimilarity[v][d][y]);
         }
      }
   }
}

void
PatchMatchStereoSlanted::free_color_dissimilarity_memory()
{
   if (!m_dissimilarity[LEFT_VIEW]) return;

   int ny = m_views[LEFT_VIEW].rows;

   int max_disparity = m_config.get_max_disparity();

   for (int v = LEFT_VIEW; v < NUM_VIEWS; v++)
   {
      CV_Assert(m_dissimilarity[v]);
      for (int d = 0; d <= max_disparity; d++)
      {
         CV_Assert(m_dissimilarity[v][d]);
         for (int y = 0; y < ny; y++)
         {
            CV_Assert(m_dissimilarity[v][d][y]);
            delete[] m_dissimilarity[v][d][y];
            m_dissimilarity[v][d][y] = NULL;
         }
         delete[] m_dissimilarity[v][d];
         m_dissimilarity[v][d] = NULL;
      }
      delete[] m_dissimilarity[v];
      m_dissimilarity[v] = NULL;
   }
}

void
PatchMatchStereoSlanted::precompute_color_dissimilarity()
{
   allocate_color_dissimilarity_memory();
   int nx = m_views[LEFT_VIEW].cols;
   int ny = m_views[LEFT_VIEW].rows;

   // todo: remove redundant code here, m_bad_disparity_cost has already been initialized
   const int s = 3;
   float alpha = m_config.get_alpha();
   float tau_color = m_config.get_tau_color();
   float tau_grad = m_config.get_tau_gradient();
   m_bad_disparity_cost = s * ((1-alpha)*tau_color + alpha*tau_grad);

   int max_disparity = m_config.get_max_disparity();
   int sign = 1;

   for (int v = LEFT_VIEW; v < NUM_VIEWS; v++)
   {
      if (RIGHT_VIEW == v) sign = -1;
#ifdef KFJ_USE_OPENMP
      #pragma omp parallel for num_threads(5)
#endif
      for (int y = 0; y < ny; y++)
      {
         const cv::Vec3f* p_this_img = m_views[v].ptr<cv::Vec3f>(y);
         const cv::Vec3f* p_other_img = m_views[1-v].ptr<cv::Vec3f>(y);

         const float* p_this_grad = m_grad_x[v].ptr<float>(y);
         const float* p_other_grad = m_grad_x[1-v].ptr<float>(y);
         for (int x = 0; x < nx; x++)
         {
            const cv::Vec3f& I_p = p_this_img[x];
            float I_p_grad = p_this_grad[x];
            for (int d = 0; d <= max_disparity; d++)
            {
               float* p_d = m_dissimilarity[v][d][y];
               int other_x = x - sign*d;
               other_x = clip_to_border(other_x, nx);
               const cv::Vec3f& I_q = p_other_img[other_x];
               float diff_intensity = (float)cv::norm(I_p, I_q, cv::NORM_L1);
               diff_intensity /= 3; // todo: need to take the average??

               diff_intensity = cv::min(diff_intensity, tau_color);

               float I_q_grad = p_other_grad[other_x];
               float diff_grad = cv::abs(I_p_grad - I_q_grad);
               diff_grad = cv::min(diff_grad, tau_grad);

               p_d[x] = (1-alpha)*diff_intensity + alpha*diff_grad;
            }
         }
      }
   }
}
