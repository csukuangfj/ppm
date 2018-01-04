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

#include "FlowFormat_F.hpp"

// format:
// float float (cols, rows)
// float float float float (the four floats are ignored)
// u,v,u,v, row by row, column by column
// refer to http://www.mia.uni-saarland.de/valgaerts/eccv10/sceneflow/read_barron.c
// The format is compatible of the ground truth files from the COPCV course.
void
FlowFormat_F::read_flow(
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
   float t;

   fread(&t, sizeof(float), 1, f);
   nc = (int)t;

   fread(&t, sizeof(float), 1, f);
   nr = (int)t;

   fread(&t, sizeof(float), 1, f);
   fread(&t, sizeof(float), 1, f);
   fread(&t, sizeof(float), 1, f);
   fread(&t, sizeof(float), 1, f);

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
FlowFormat_F::write_flow(
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

   int nr = u_.rows; // height
   int nc = u_.cols; // width
   float t;

   t = (float)nc;
   fwrite(&t, sizeof(float), 1, f);

   t = (float)nr;
   fwrite(&t, sizeof(float), 1, f);

   t = (float)nc;
   fwrite(&t, sizeof(float), 1, f);

   t = (float)nr;
   fwrite(&t, sizeof(float), 1, f);

   t = 0;
   fwrite(&t, sizeof(float), 1, f);
   fwrite(&t, sizeof(float), 1, f);

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
         t = u_ref(r,c);
         fwrite(&t, sizeof(float), 1, f);

         t = v_ref(r,c);
         fwrite(&t, sizeof(float), 1, f);
      }
   }
#endif

   fclose(f);
}
