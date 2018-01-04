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

#include "SiftDescriptor.hpp"

// modified from https://people.csail.mit.edu/celiu/SIFTflow/
// refer to https://github.com/csukuangfj/sift-flow/blob/master/2011/mexDenseSIFT/ImageFeature.h#L26
void
SiftDescriptor::compute_sift_descriptor(
      const cv::Mat& image_,
      cv::Mat& imsift,
      int cell_size_, // = 2
      int step_size_, // = 1
      bool is_boundary_included_, // = true,
      int num_bins_// = 8
)
{
   if (cell_size_ <= 0) cell_size_ = 2;

   // this parameter controls decay of the gradient energy falls into a bin
   // run SIFT_weightFunc.m to see why alpha = 9 is the best value
   int alpha = 9;

   int width = image_.cols;
   int height = image_.rows;
   int nchannels = image_.channels();
   int nPixels = width*height;

   cv::Mat img;
   image_.convertTo(img, CV_32F);

   cv::Mat kx = (cv::Mat_<float>(1,5) << 1,-8,0,8,-1); // x-derivative kernel
   cv::Mat ky = (cv::Mat_<float>(5,1) << 1,-8,0,8,-1); // y-derivative kernel
   kx /= 12;
   ky /= 12;

   cv::Mat imdx,imdy;

   // compute the derivatives;
   cv::filter2D(img, imdx, CV_32F, kx, cv::Point(2,0));
   cv::filter2D(img, imdy, CV_32F, ky, cv::Point(0,2));

   // get the maximum gradient over the channels and estimate the normalized gradient
   cv::Mat magsrc(height, width, img.type());
   cv::Mat mag(height, width, CV_32FC1);
   cv::Mat gradient(height,width, CV_32FC2);

   float* magsrc_pData = magsrc.ptr<float>(0);
   float* imdx_pData = imdx.ptr<float>(0);
   float* imdy_pData = imdy.ptr<float>(0);
   float* gradient_pData = gradient.ptr<float>(0);
   float* mag_pData = mag.ptr<float>(0);

#if defined(SIFT_OPENMP)
#pragma omp parallel for
#endif
   for(int i = 0;i < nPixels; i++)
   {
      int offset = i * nchannels;
      for(int j = 0;j < nchannels; j++)
         magsrc_pData[offset+j] = (float)(sqrt(imdx_pData[offset+j]*imdx_pData[offset+j]+imdy_pData[offset+j]*imdy_pData[offset+j]));
      float Max = magsrc_pData[offset];
      if(Max > 1e-7)
      {
         gradient_pData[i*2] = imdx_pData[offset]/Max;
         gradient_pData[i*2+1] = imdy_pData[offset]/Max;
      }
      for(int j = 1; j < nchannels; j++)
      {
         if(magsrc_pData[offset+j]>Max)
         {
            Max = magsrc_pData[offset+j];
            gradient_pData[i*2] = imdx_pData[offset+j]/Max;
            gradient_pData[i*2+1] = imdy_pData[offset+j]/Max;
         }
      }
      mag_pData[i] = Max;
   }

   // get the pixel-wise energy for each orientation band
   cv::Mat imband(height, width, CV_32FC(num_bins_));
   float* imband_pData = imband.ptr<float>(0);
   float theta = (float)M_PI*2/num_bins_;

#if defined(SIFT_OPENMP)
#pragma omp parallel for
#endif
   for(int k = 0; k < num_bins_; k++)
   {
      float _cos, _sin, temp;
      _sin = (float)sin(theta*k);
      _cos = (float)cos(theta*k);
      for(int i = 0; i < nPixels; i++)
      {
         temp = cv::max(gradient_pData[i*2]*_cos + gradient_pData[i*2+1]*_sin, 0.0f);
         //if(alpha>1) // the condition is always true, as alpha is set to a fixed value 9
            temp = (float)cv::pow(temp,alpha);
         imband_pData[i*num_bins_+k] = temp*mag_pData[i];
      }
   }

   // filter out the SIFT feature
   cv::Mat_<float> filter(cell_size_*2+1,1);
   filter[0][0] = filter[cell_size_+1][0] = 0.25;
   for(int i = 1; i < cell_size_ + 1; i++)
      filter[i+1][0] = 1;
   for(int i = cell_size_ + 2; i < cell_size_*2 + 1; i++)
      filter[i][0] = 0;

   cv::Mat imband_cell;
   cv::sepFilter2D(imband, imband_cell, CV_32F, filter, filter);
   float* imband_cell_pData = imband_cell.ptr<float>(0);

   // allocate buffer for the sift image
   int siftdim = num_bins_*16;
   int sift_width,sift_height,x_shift=0,y_shift=0;

   sift_width = width/step_size_;
   sift_height = height/step_size_;

   if(!is_boundary_included_)
   {
      sift_width = (width-4*cell_size_)/step_size_;
      sift_height= (height-4*cell_size_)/step_size_;
      x_shift = 2*cell_size_;
      y_shift = 2*cell_size_;
   }

   imsift.create(sift_height, sift_width, CV_8UC(siftdim));
   uchar* imsift_pData = imsift.ptr<uchar>(0);

#if defined(SIFT_OPENMP)
#pragma omp parallel for
#endif
   for(int i = 0; i < sift_height; i++)
   {
      // now sample to get SIFT image
      cv::Mat sift_cell(siftdim, 1, CV_32FC1);
      float* sift_cell_pData = sift_cell.ptr<float>(0);

      for(int j = 0; j < sift_width; j++)
      {
         int count = 0;
         for(int ii = -1; ii <= 2; ii++)
            for(int jj = -1; jj <= 2; jj++)
            {
               int y = cv::min(cv::max(y_shift+i*step_size_+ii*cell_size_,0), height-1);
               int x = cv::min(cv::max(x_shift+j*step_size_+jj*cell_size_,0), width-1);

               // the following code is the same as the above two for debugging purposes
               //int y = y_shift+i*stepSize+ii*cellSize;
               //int x = x_shift+j*stepSize+jj*cellSize;
               //if (x<0 || x>=width)
               //	x = __min(__max(x,0),width-1);
               //if (y<0 || y>=height)
               //	y= __min(__max(y,0),height-1);

               memcpy(sift_cell_pData+count*num_bins_,imband_cell_pData+(y*width+x)*num_bins_,sizeof(float)*num_bins_);
               count++;
            }
         // normalize the SIFT descriptor
         double mag = cv::norm(sift_cell, cv::NORM_L2);
         int offset = (i*sift_width+j)*siftdim;
         //memcpy(imsift.pData+offset,sift_cell.pData,sizeof(double)*siftdim);
         for(int k = 0;k<siftdim;k++)
            imsift_pData[offset+k] =  (uchar)cv::min(sift_cell_pData[k]/(mag+0.01)*255,255.);//(unsigned char) __min(sift_cell.pData[k]/mag*512,255);
      }//*/
   }
}

void
SiftDescriptor::pca_visualisation(
      const cv::Mat& sift_image_,
      cv::Mat& out_img_,
      const std::string& filename_
)
{
   FILE* f = fopen(filename_.c_str(), "r");
   if (!f)
   {
      std::cerr << "File '" << filename_ << "' cannot be opened!"
                << std::endl;
      return;
   }

   float rows, cols;

   fread(&rows, sizeof(float), 1, f);
   fread(&cols, sizeof(float), 1, f);

   cv::Mat m;
   //m.create(rows, 1, CV_32FC((int)cols));
   m.create(rows, cols, CV_32FC1);
   fread(m.data, sizeof(float), rows*cols, f);

   fclose(f);

   cv::Mat m2(m, cv::Range(0,3));

   cv::Mat con = (cv::Mat_<float>(3,3) << 1,1,1, 1,-1,0, 0.5,0.5,-1);
   con = con.inv();
   //std::cout << con << std::endl;
   con = con * m2;

   int ny = sift_image_.rows;
   int nx = sift_image_.cols;
   int nc = sift_image_.channels();
   CV_Assert(nc == (int)cols);

   cv::Mat img(ny, nx, CV_32FC3);

   float min_v = FLT_MAX, max_v = FLT_MIN;

   for (int y = 0; y < ny; y++)
   {
      for (int x = 0; x < nx; x++)
      {
         int offset = (y*nx + x)*nc;
         uchar* d = (uchar *)sift_image_.data + offset;
         cv::Mat t(nc, 1, CV_8UC1, d);
         cv::Mat t2 = t.clone();
         t2.convertTo(t2, CV_32F);

         cv::Mat rgb = con * t2;
         float r = rgb.at<float>(0,0);
         float g = rgb.at<float>(1,0);
         float b = rgb.at<float>(2,0);

         if (r > max_v) max_v = r;
         if (g > max_v) max_v = g;
         if (b > max_v) max_v = g;

         if (r < min_v) min_v = r;
         if (g < min_v) min_v = g;
         if (b < min_v) min_v = b;

         img.at<cv::Vec3f>(y,x) = cv::Vec3f(b,g,r);
      }
   }

   img = img - min_v;
   img = img / (max_v - min_v);

   out_img_ = img;
}
