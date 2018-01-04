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
#include <stdio.h>
#include "StereoFormatPfm.hpp"

static bool
is_little_endian()
{
   int i = 0x01020304;
   char* p = (char*)(&i);
   return (*p == 0x04);
}

static void
swap_bytes(float *f)
{
   char* p = (char*)f;
   cv::swap(p[0], p[3]);
   cv::swap(p[1], p[2]);
}


// refer to http://netpbm.sourceforge.net/doc/pfm.html
// and http://vision.middlebury.edu/stereo/eval3/MiddEval3-newFeatures.html
// invalid disparity are represented as inf

/*
Pf or PF
width height
scale
array of data

Pf - grayscale
PF - color (RGB)
scale - negative(little endian), positive(big endian)
array of data: from bottom to top, left to right.
Note that the data format for PGM is: from top to bottom, left to right
 */
cv::Mat
StereoFormatPfm::read_disparity_from_file(
      const cv::String &filename_,
      cv::OutputArray& valid_,
      float* options_,
      int /*options_len_*/
)
{
   cv::Mat res;
   FILE* f = fopen(filename_.c_str(), "rb");
   if (!f)
   {
      std::string s;
      s = "Open '" + filename_ + "' failed!";
      printf("%s\n", s.c_str());
      return res;
   }

   // read header
   char header_[3] = {0};
   fread(header_, 1, 2, f);
   std::string header(header_);
   if ((header != "Pf") && (header != "PF"))
   {
      std::string s;
      s = "Unsupported format: '" + header + "'!";
      printf("%s\n", s.c_str());

      fclose(f);
      return res;
   }

   char help;
   fread(&help, 1, 1, f); // skip 0x0a

   int width, height;
   fscanf(f, "%d %d", &width, &height);
   fread(&help, 1, 1, f); // skip 0x0a

   float scale_;
   fscanf(f, "%f", &scale_);
   fread(&help, 1, 1, f); // skip 0x0a

   if (options_) *options_ = scale_;

   bool need_swap = (is_little_endian() != (scale_ < 0));

#if 0
   char msg[100];
   snprintf(msg, sizeof(msg),
            "format: %s\nwidth:%d\nheight:%d\nscale:%f\nneed_swap:%s\n",
            header.c_str(), width, height, scale_,
            (need_swap? "true": "false"));

   printf("%s\n", msg);
#endif

   cv::Mat valid;

   valid.create(height, width, CV_8UC1);
   valid = 1;

   if ("Pf" == header)
   {  // Pf, grayscale
      res.create(height, width, CV_32FC1);
      for (int j = height - 1; j >= 0; j--)
      {
         for (int i = 0; i < width; i++)
         {
            float h;
            fread(&h, sizeof(float), 1, f);
            if (need_swap) swap_bytes(&h);
            res.at<float>(j,i) = h;
            if (std::isinf(h))
            {
               valid.at<uchar>(j,i) = 0;
               res.at<float>(j,i) = 0; // invalid disparity is set to 0
            }
         }
      }
   }
   else
   {  // PF, color RGB
      res.create(height, width, CV_32FC3);
      for (int j = height - 1; j >= 0; j--)
      {
         for (int i = 0; i < width; i++)
         {
            float h1, h2, h3;
            fread(&h1, sizeof(float), 1, f);
            fread(&h2, sizeof(float), 1, f);
            fread(&h3, sizeof(float), 1, f);
            if (need_swap)
            {
               swap_bytes(&h1); swap_bytes(&h2); swap_bytes(&h3);
            }
            res.at<cv::Vec3f>(j,i) = cv::Vec3f(h3, h2, h1);

            if (std::isinf(h1) || std::isinf(h2) || std::isinf(h3))
            {
               valid.at<uchar>(j,i) = 0;
               res.at<cv::Vec3f>(j,i) = cv::Vec3f(0,0,0); // invalid disparity is set to 0
            }
         }
      }
   }

   if (valid_.needed())
   {
      valid.copyTo(valid_);
   }

   fclose(f);

   return res;
}

/**
 * Default to use little endian, scale is -1.
 *
 * @param filename_
 * @param disparity_
 * @param options_      [in] options[0] is the scale
 * @param options_len_
 */
void
StereoFormatPfm::write_disparity_to_file(
      const cv::String &filename_,
      const cv::Mat &disparity_,
      const float* options_,
      int /*options_len_*/
)
{
   CV_Assert((disparity_.type() == CV_32FC1) ||
             (disparity_.type() == CV_32FC3));


   FILE* f = fopen(filename_.c_str(), "wb");
   if (!f)
   {
      fprintf(stderr, "Create file '%s' failed!\n", filename_.c_str());
      return;
   }

   fputc('P', f);

   bool is_color = (disparity_.channels() == 3);
   if (is_color) fputc('F', f);
   else fputc('f', f);

   fputc(0x0a, f);

   int cols = disparity_.cols;
   int rows = disparity_.rows;
   fprintf(f, "%d %d", cols, rows);
   fputc(0x0a, f);

   float scale = -1; // little endian by default
   if (options_) scale = options_[0];
   fprintf(f, "%f", scale); // 6 decimal by default
   fputc(0x0a, f);

   /*
    * is_little_endian   scale     need_swap
    *    true             -1         false
    *    true              1         true
    *    false            -1         true
    *    false             1         false
    */
   bool need_swap = (is_little_endian() != (scale < 0));

   if (is_color)
   {
      // (0,0) is at the bottom left -> start from rows-1
      for (int j = rows-1; j >= 0; j--)
      {
         const cv::Vec3f* p = disparity_.ptr<cv::Vec3f>(j);
         for (int i = 0; i < cols; i++)
         {
            if (need_swap)
            {
               float h;
               h = p[i][2]; swap_bytes(&h);
               fwrite(&h, sizeof(float), 1, f);

               h = p[i][1]; swap_bytes(&h);
               fwrite(&h, sizeof(float), 1, f);

               h = p[i][0]; swap_bytes(&h);
               fwrite(&h, sizeof(float), 1, f);
            }
            else
            {
               fwrite(p[i].val+2, sizeof(float), 1, f); // R
               fwrite(p[i].val+1, sizeof(float), 1, f); // G
               fwrite(p[i].val+0, sizeof(float), 1, f); // B
            }
         }
      }
   }
   else
   {
      for (int j = rows-1; j >= 0; j--)
      {
         const float* p = disparity_.ptr<float>(j);
         for (int i = 0; i < cols; i++)
         {
            float h = p[i];
            if (need_swap) swap_bytes(&h);
            fwrite(&h, sizeof(float), 1, f);
         }
      }
   }

   //printf("Write file '%s' successfully\n", filename_.c_str());

   fclose(f);
}
