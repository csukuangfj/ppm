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
#include "pm_common.hpp"

/**
 * Check whether x is in the interval [0, w).
 *  if 0 <= x < w, then returns true
 *  else returns false
 * @param x [in]
 * @param w [in]
 * @return true if x is in [0,w), else false
 */
bool
is_inside(int x, int w)
{
   return (x >= 0) && (x < w);
}

/**
 * Compute the cost of the new flow field and
 * update the old flow field if necessary.
 *
 * @param f_            [in]  Frame 1, any data type supported by cv::norm
 * @param g_            [in]  Frame 2, any data type supported by cv::norm
 * @param x_            [in]  x coordinate of the current pixel in f_ (horizontal, i.e, column number)
 * @param y_            [in]  y coordinate of the current pixel in f_ (vertical, i.e., row number)
 * @param u_old_        [in,out] The old flow field in x-direction in f_
 * @param v_old_        [in,out] The old flow field in y-direction in f_
 * @param cost_old_     [in]     The old match cost for the flow field (u_old_,v_old_) at position (x_,y_)
 * @param u_new_        [in]     The new flow field in x-direction
 * @param v_new_        [in]     The new flow field in y-direction
 * @param cost_func_ptr_    [in] Pointer to the function for computing matching cost
 * @param half_patch_size_  [in] Half patch size. 0 means to use only a single point
 *
 * @return true if the cost is improved. false if the cost is not changed.
 */
bool
improve_cost(const cv::Mat& f_,
             const cv::Mat& g_,
             int x_,
             int y_,
             float& u_old_,
             float& v_old_,
             float& cost_old_,
             float u_new_,
             float v_new_,
             cv::Ptr<MatchCost> cost_func_ptr_,
             int half_patch_size_
)
{
   bool res = false;
   int nx = f_.cols;
   int ny = f_.rows;

   float cost_new = cost_old_ + 1;
#if 1
   int other_x = cvRound(x_ + u_new_);
   int other_y = cvRound(y_ + v_new_);

   if (!is_inside(other_x, nx) || !is_inside(other_y, ny))
   {
      return res;
   }

   cost_new = (float)cost_func_ptr_->compute_cost(f_,
                                                  g_,
                                                  cv::Point(x_, y_),
                                                  cv::Point(other_x, other_y),
                                                  half_patch_size_);
#else
   if (half_patch_size_)
   {
      int other_x = cvRound(x_ + u_new_);
      int other_y = cvRound(y_ + v_new_);

      if (!is_inside(other_x, nx) || !is_inside(other_y, ny))
      {
         return res;
      }

      cost_new = (float)cost_func_ptr_->compute_cost(f_,
                                                     g_,
                                                     cv::Point(x_, y_),
                                                     cv::Point(other_x, other_y),
                                                     half_patch_size_);
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

      int x_left = (int)(x_ + u_new_);
      int x_right = x_left + 1;
      float dx = x_ + u_new_ - x_left;

      int y_top = (int)(y_+v_new_);
      int y_bottom = y_top + 1;
      float dy = y_ + v_new_ - y_top;
      float c1 = 0, c2 = 0, c3 = 0, c4 = 0;


      if (!is_inside(x_left, nx) || !is_inside(x_right, nx) || !is_inside(y_top, ny) || !is_inside(y_bottom, ny)) return false;

      c1 = (float) cost_func_ptr_->compute_cost(f_,
                                                g_,
                                                cv::Point(x_, y_),
                                                cv::Point(x_left, y_top),
                                                half_patch_size_);

      c2 = (float) cost_func_ptr_->compute_cost(f_,
                                                g_,
                                                cv::Point(x_, y_),
                                                cv::Point(x_right, y_top),
                                                half_patch_size_);

      c3 = (float) cost_func_ptr_->compute_cost(f_,
                                                g_,
                                                cv::Point(x_, y_),
                                                cv::Point(x_left, y_bottom),
                                                half_patch_size_);

      c4 = (float) cost_func_ptr_->compute_cost(f_,
                                                g_,
                                                cv::Point(x_, y_),
                                                cv::Point(x_right, y_bottom),
                                                half_patch_size_);

      cost_new = c1*(1-dx)*(1-dy) +
                 c2*   dx *(1-dy) +
                 c3*(1-dx)*   dy  +
                 c4*   dx *   dy;
#if 0
         printf("updated: \n"
                " c1: %.3f\n"
                " c2: %.3f\n"
                " c3: %.3f\n"
                " c4: %.3f\n"
                " x_left: %d\n"
                " x_: %d\n"
                " u_new_: %.2f\n"
                " y_top: %d\n"
                " y_: %d\n"
                " v_new: %.2f\n"
                " dx: %.3f\n"
                " dy: %.3f\n",
                c1,c2,c3,c4,
                x_left, x_, u_new_,
                y_top, y_, v_new_,
                dx,dy
         );
#endif

   }
#endif

   if (cost_new < cost_old_)
   {
      cost_old_ = cost_new;
      u_old_ = u_new_;
      v_old_ = v_new_;
      res = true;
   }
   return res;
}

void
compute_cost(
      const cv::Mat& u_,
      const cv::Mat& v_,
      const cv::Mat& image1_,
      const cv::Mat& image2_,
      cv::Mat& cost_,
      const cv::Ptr<MatchCost> cost_func_ptr_,
      const cv::Mat& seeds_,
      int half_patch_size_
)
{
   cost_.create(u_.size(), CV_32FC1);
   cost_ = 1e10; // initialized to a very large cost
   int ny = image1_.rows;
   int nx = image1_.cols;

   int num_seeds = seeds_.rows;

   for (int i = 0; i < num_seeds; i++)
   {
      int y = seeds_.at<int>(i, 1);
      int x = seeds_.at<int>(i, 0);

      float u = u_.at<float>(y, x);
      float v = v_.at<float>(y, x);
#if 1
      int x2 = cvRound(x + u);
      int y2 = cvRound(y + v);

      if (!is_inside(x2, nx) || !is_inside(y2, ny)) continue; // invalid flows are already penalized with cost 1e10

      float c = (float) cost_func_ptr_->compute_cost(image1_,
                                                     image2_,
                                                     cv::Point(x, y),
                                                     cv::Point(x2, y2),
                                                     half_patch_size_);
      cost_.at<float>(y, x) = c;
#else
      if (half_patch_size_)
      {
         int x2 = cvRound(x + u);
         int y2 = cvRound(y + v);

         if (!is_inside(x2, nx) || !is_inside(y2, ny)) continue; // invalid flows are already penalized with cost 1e10

         float c = (float) cost_func_ptr_->compute_cost(image1_,
                                                        image2_,
                                                        cv::Point(x, y),
                                                        cv::Point(x2, y2),
                                                        half_patch_size_);
         cost_.at<float>(y, x) = c;
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
            c1 = (float) cost_func_ptr_->compute_cost(image1_,
                                                      image2_,
                                                      cv::Point(x, y),
                                                      cv::Point(x_left, y_top),
                                                      half_patch_size_);
            updated = true;
         }

         if (is_inside(x_right, nx) && is_inside(y_top, ny))
         {
            c2 = (float) cost_func_ptr_->compute_cost(image1_,
                                                      image2_,
                                                      cv::Point(x, y),
                                                      cv::Point(x_right, y_top),
                                                      half_patch_size_);
            updated = true;
         }

         if (is_inside(x_left, nx) && is_inside(y_bottom, ny))
         {
            c3 = (float) cost_func_ptr_->compute_cost(image1_,
                                                      image2_,
                                                      cv::Point(x, y),
                                                      cv::Point(x_left, y_bottom),
                                                      half_patch_size_);
            updated = true;
         }

         if (is_inside(x_right, nx) && is_inside(y_bottom, ny))
         {
            c4 = (float) cost_func_ptr_->compute_cost(image1_,
                                                      image2_,
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

            cost_.at<float>(y, x) = c;
         }
      }
#endif
   }
}
