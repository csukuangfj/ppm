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
#include <math.h>
#include <stdlib.h>
#include <opencv2/core.hpp>

#include "FlowVisualization_FLO.hpp"

static void compute_color(float fx, float fy, uchar *pix);

void
FlowVisualization_FLO::flow_to_image(
     const cv::Mat& u_,
     const cv::Mat& v_,
     cv::Mat& image_,
     int max_disp_,
     const cv::Mat& mask_
)
{
   CV_Assert(u_.size == v_.size);
   CV_Assert(u_.type() == v_.type());
   CV_Assert(u_.type() == CV_32FC1);

   CV_Assert(mask_.empty() ||
             ((mask_.type() == CV_8UC1) && (mask_.size() == u_.size())));
   cv::Mat mask = mask_;
   if (mask.empty())
   {
      mask = cv::Mat::ones(u_.size(), CV_8UC1);
   }

   image_ = cv::Mat::zeros(u_.size(), CV_8UC3);

   if (max_disp_ <= 0)
   {
      cv::Mat tmp;
      cv::magnitude(u_, v_, tmp);

      double max_val;
      cv::minMaxIdx(tmp, nullptr, &max_val);
      max_disp_ = (int)max_val;
   }

   float scaling = 1.0f/max_disp_;

   int nr = u_.rows;
   int nc = u_.cols;

   const cv::Mat_<float>& u_ref = (const cv::Mat_<float>&)u_;
   const cv::Mat_<float>& v_ref = (const cv::Mat_<float>&)v_;
   cv::Mat_<cv::Vec3b>& out_ref = (cv::Mat_<cv::Vec3b>&)image_;
   const cv::Mat_<uchar>& mask_ref = (const cv::Mat_<uchar>&)mask;

   for (int j = 0; j < nr; j++)
   {
      for (int i = 0; i < nc; i++)
      {
         float u_ij = u_ref(j,i) * scaling;
         float v_ij = v_ref(j,i) * scaling;
         if (!mask_ref(j,i))
         {
            continue;
         }
         compute_color(u_ij, v_ij, out_ref(j,i).val);
      }
   }
}

/*
Refer to colortest.cpp from
 http://vision.middlebury.edu/flow/code/flow-code/
 */
void
FlowVisualization_FLO::get_standard_color_wheel(cv::Mat& image_)
{
   int image_size = 300; // it was 151 by default
   int true_range = 10;
   float range = 1.04f * true_range;

   image_.create(image_size, image_size, CV_8UC3);
   cv::Mat_<cv::Vec3b>& out_ref = (cv::Mat_<cv::Vec3b>&)image_;

   int s2 = image_size/2;
   for (int y = 0; y < image_size; y++) {
      for (int x = 0; x < image_size; x++) {
         float fx = (float)x / (float)s2 * range - range;
         float fy = (float)y / (float)s2 * range - range;
         if (x == s2 || y == s2) // make black coordinate axes
            continue;
         uchar* pix = out_ref(y,x).val;
         //fx = rintf(fx);
         //fy = rintf(fy);
         compute_color(fx/true_range, fy/true_range, pix);
      }
   }
   int ir = true_range;
   int ticksize = image_size < 120 ? 1 : 2;
   for (int k = -ir; k <= ir; k++) {
      int ik = (int)(k / range * s2) + s2;
      for (int t = -ticksize; t <= ticksize; t++) {
         uchar *pix;
         pix = out_ref(ik,s2+t).val;
         pix[0] = pix[1] = pix[2] = 0;

         pix = out_ref(s2+t, ik).val;
         pix[0] = pix[1] = pix[2] = 0;
      }
   }
}


// colorcode.cpp
//
// Color encoding of flow vectors
// adapted from the color circle idea described at
//   http://members.shaw.ca/quadibloc/other/colint.htm
//
// Daniel Scharstein, 4/2007
// added tick marks and out-of-range coding 6/05/07
typedef unsigned char uchar;

static int ncols = 0;
static const int MAXCOLS = 60;
static int colorwheel[MAXCOLS][3];

static void
setcols(int r, int g, int b, int k)
{
   colorwheel[k][0] = r;
   colorwheel[k][1] = g;
   colorwheel[k][2] = b;
}

static void
make_color_wheel()
{
   // relative lengths of color transitions:
   // these are chosen based on perceptual similarity
   // (e.g. one can distinguish more shades between red and yellow
   //  than between yellow and green)
   int RY = 15;
   int YG = 6;
   int GC = 4;
   int CB = 11;
   int BM = 13;
   int MR = 6;
   ncols = RY + YG + GC + CB + BM + MR;
   if (ncols > MAXCOLS)
      exit(1);
   int i;
   int k = 0;
   for (i = 0; i < RY; i++) setcols(255, 255*i/RY, 0, k++);
   for (i = 0; i < YG; i++) setcols(255-255*i/YG, 255, 0, k++);
   for (i = 0; i < GC; i++) setcols(0, 255, 255*i/GC, k++);
   for (i = 0; i < CB; i++) setcols(0, 255-255*i/CB, 255, k++);
   for (i = 0; i < BM; i++) setcols(255*i/BM, 0, 255, k++);
   for (i = 0; i < MR; i++) setcols(255, 0, 255-255*i/MR, k++);
}

static void
compute_color(float fx, float fy, uchar *pix)
{
   if (ncols == 0)
      make_color_wheel();

   float rad = (float)sqrt(fx * fx + fy * fy);
   float a = (float)atan2(-fy, -fx) / (float)M_PI;
   float fk = (a + 1.0f) / 2.0f * (ncols-1);
   int k0 = (int)fk;
   int k1 = (k0 + 1) % ncols;
   float f = fk - k0;
   //f = 0; // uncomment to see original color wheel
   for (int b = 0; b < 3; b++) {
      float col0 = colorwheel[k0][b] / 255.0f;
      float col1 = colorwheel[k1][b] / 255.0f;
      float col = (1 - f) * col0 + f * col1;
      if (rad <= 1)
         col = 1 - rad * (1 - col); // increase saturation with radius
      else
         col *= .75f; // out of range
      int help = (int)(255.0f * col);
      pix[2 - b] = (help > 255) ? (uchar)255 : (uchar)help;
   }
}
