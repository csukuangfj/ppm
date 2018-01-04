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
#include "PatchMatchStereoImplSlantedPlane.hpp"

//static const double g_double_EPS = 1e-7;
static const float g_float_EPS = 1e-5;
/*
 0   1   2   3   4  5
 nx  ny  nz  a   b  c
 */

static void
init_abc(
      float* p_out_,
      float nx_,
      float ny_,
      float nz_,
      int x_,
      int y_,
      float z_
)
{
   cv::Vec3f norm_vec(nx_, ny_, nz_);

   float mag = (float)cv::norm(norm_vec, cv::NORM_L2);

   mag = cv::max(g_float_EPS, mag);

   norm_vec = norm_vec/mag;

   float nx,ny,nz;
   nx = norm_vec[0];
   ny = norm_vec[1];
   nz = norm_vec[2];

   if (cv::abs(nz) < g_float_EPS) // avoid dividing by 0
   {
      if (nz < 0) nz = -g_float_EPS;
      else nz = g_float_EPS;
   }

   float a,b,c;
   a = -nx/nz;
   b = -ny/nz;
   c = (nx*x_ + ny*y_ + nz*z_)/nz;

   p_out_[0] = nx;
   p_out_[1] = ny;
   p_out_[2] = nz;
   p_out_[3] = a;
   p_out_[4] = b;
   p_out_[5] = c;
}

PatchMatchStereoImplSlantedPlane::PatchMatchStereoImplSlantedPlane()
{
   /*
      0: nx
      1: ny
      2: nz
      3: disparity
      4: x
      5: y
    */
   set_num_properties(6);

   set_min_search_value(0.1f);
}

void
PatchMatchStereoImplSlantedPlane::property_random_init(
      float *p_property_,
      float value_range_,
      int x_,
      int y_
)
{

   float z = cv::theRNG().uniform(0.0f, value_range_); // disparity is always positive

   float nx = cv::theRNG().uniform(-1.0f, 1.0f);
   float ny = cv::theRNG().uniform(-1.0f, 1.0f);
   float nz = cv::theRNG().uniform(-1.0f, 1.0f);

   init_abc(p_property_, nx, ny, nz, x_, y_, z);
}

float
PatchMatchStereoImplSlantedPlane::compute_disparity(
      const float *p_property_,
      int x_,
      int y_,
      bool /*is_left_view_*/
)
{
   float a = p_property_[3];
   float b = p_property_[4];
   float c = p_property_[5];
   float res;
   res = a*x_ + b*y_ + c;
   return res;
}

void
PatchMatchStereoImplSlantedPlane::property_view_conversion(
      const float *p_property_in_,
      float *p_property_out_,
      int x_,
      int y_,
      float z_,
      bool /*is_left_view*/
)
{
#if 0
   PatchMatchStereoSlantedPlane try_plane;
   try_plane.init(best_plane.get_unit_norm_vector(), cv::Vec3d(other_x, y, d_));
   improve_cost(other_x, y, other_cost_best, other_plane_best, try_plane, (ViewIndex)(1-v));
#endif

   float nx = p_property_in_[0];
   float ny = p_property_in_[1];
   float nz = p_property_in_[2];

   init_abc(p_property_out_, nx, ny, nz, x_, y_, z_);
}

void
PatchMatchStereoImplSlantedPlane::random_search(
      const float *p_old_property_,
      float *p_try_property_,
      float delta_,
      int x_,
      int y_,
      int num_iter_,
      bool is_left_view_
)
{
#if 1
   float old_z = compute_disparity(p_old_property_, x_, y_, is_left_view_);
   float new_z = old_z + cv::theRNG().uniform(-delta_, delta_);
#else
   float new_z = cv::theRNG().uniform(-delta_, delta_);
#endif

   float delta_n = 1.0f / (1<<num_iter_);
#if 1
   float new_nx = p_old_property_[0] + cv::theRNG().uniform(-delta_n, delta_n);
   float new_ny = p_old_property_[1] + cv::theRNG().uniform(-delta_n, delta_n);
   float new_nz = p_old_property_[2] + cv::theRNG().uniform(-delta_n, delta_n);
#else
   float new_nx = cv::theRNG().uniform(-1.0f, 1.0f);
   float new_ny = cv::theRNG().uniform(-1.0f, 1.0f);
   float new_nz = cv::theRNG().uniform(-1.0f, 1.0f);
#endif

   init_abc(p_try_property_, new_nx, new_ny, new_nz, x_, y_, new_z);
}

void
PatchMatchStereoImplSlantedPlane::copy_properties(
      const float *p_property_in_,
      float *p_property_out_,
      int x_,
      int y_,
      float z_
)
{
   float nx = p_property_in_[0];
   float ny = p_property_in_[1];
   float nz = p_property_in_[2];

   init_abc(p_property_out_, nx, ny, nz, x_, y_, z_);
}
