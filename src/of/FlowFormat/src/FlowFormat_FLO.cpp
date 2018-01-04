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

#include "FlowFormat_FLO.hpp"

/*
Refer to http://vision.middlebury.edu/flow/code/flow-code/README.txt
 0-3 "PIEH" in litte endian, i.e, the first byte is 'P'
 4-7, integer, width in little endian
 8-11, integer, height in little endian
 12-end, width*height*2*4 bytes, u(row0,col0), v(row0,col0),u(row0,col1) ....
*/
void
FlowFormat_FLO::read_flow(
     const std::string& filename_,
     cv::Mat& u_,
     cv::Mat& v_,
     cv::OutputArray& valid_
)
{
   FILE* f = fopen(filename_.c_str(), "rb");
   if (!f)
   {
      std::cerr << "Open '" << filename_ << "'" << " failed!" << std::endl;
      return;
   }

   int nr; // number of rows, i.e., image height
   int nc; // number of columns, i.e., image width

   char magic_number[5] = {0};
   fread(magic_number, 1, 4, f);
   if (strncmp(magic_number, "PIEH", 4) != 0)
   {
      std::cerr << "Unknown flow format!" << std::endl
                << "Expected magic number: 'PIEH'"
                << "Actual magic number: '" << magic_number << "'"
                << std::endl;
      fclose(f);
      return;
   }

   fread(&nc, sizeof(int), 1, f);
   fread(&nr, sizeof(int), 1, f);

   cv::Mat valid;
   if (valid_.kind() == cv::_InputArray::NONE)
   {
      valid.create(nr, nc, CV_8UC1);
   }
   else
   {
      valid_.create(nr, nc, CV_8UC1);
      valid = valid_.getMat();
   }
   valid.setTo(1);

#if 1
   cv::Mat uv(nr, nc, CV_32FC2);
   fread(uv.data, sizeof(float), ((size_t)nr)*nc*2, f);
   cv::Mat mats[2];
   cv::split(uv, mats);
   u_ = mats[0];
   v_ = mats[1];
#else
   u_.create(nr, nc, CV_32FC1);
   v_.create(nr, nc, CV_32FC1);
   cv::Mat_<float>& u_ref = (cv::Mat_<float>&)u_;
   cv::Mat_<float>& v_ref = (cv::Mat_<float>&)v_;

   for (int r = 0; r < nr; r++)
   {
      for (int c = 0; c < nc; c++)
      {
         float t;
         fread(&t, sizeof(float), 1, f);
         u_ref(r,c) = t;

         fread(&t, sizeof(float), 1, f);
         v_ref(r,c) = t;
      }
   }
#endif

   fclose(f);
}

void
FlowFormat_FLO::write_flow(
      const std::string& filename_,
      const cv::Mat& u_,
      const cv::Mat& v_,
      const cv::Mat& /*valid_*/
)
{
   CV_Assert(u_.size == v_.size);
   CV_Assert(u_.type() == v_.type());
   CV_Assert(u_.type() == CV_32FC1);

   FILE* f = fopen(filename_.c_str(), "wb");
   if (!f)
   {
      std::cerr << "Write to '" << filename_ << "'" << " failed!" << std::endl;
      return;
   }

   int nr = u_.rows;
   int nc = u_.cols;

   char magic_number[4] = {'P', 'I', 'E', 'H'}; // no need to end with '\0'

   fwrite(magic_number, 1, sizeof(magic_number), f);
   fwrite(&nc, sizeof(int), 1, f);
   fwrite(&nr, sizeof(int), 1, f);

#if 1
   cv::Mat uv;
   std::vector<cv::Mat> mats{u_, v_};
   cv::merge(mats, uv);
   fwrite(uv.data, sizeof(float), ((size_t)nr)*nc*2, f);
#else
   const cv::Mat_<float>& u_ref = (const cv::Mat_<float>&)u_;
   const cv::Mat_<float>& v_ref = (const cv::Mat_<float>&)v_;

   for (int r = 0; r < nr; r++)
   {
      for (int c = 0; c < nc; c++)
      {
         float t;
         t = u_ref(r,c);
         fwrite(&t, sizeof(float), 1, f);

         t = v_ref(r,c);
         fwrite(&t, sizeof(float), 1, f);
      }
   }
#endif

   fclose(f);
}
