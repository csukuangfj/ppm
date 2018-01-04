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
#include <opencv2/imgproc.hpp>

static void
get_census_8(
      const uchar* data_,
      uchar val_,
      uchar& census_
)
{
   census_ = 0;
   for (int i = 0; i < 8; i++)
   {
      if (data_[i] < val_)
      {
         census_ |= 1;
      }
      if (i != 7)
      {
         census_ <<= 1;
      }
   }
}

static void
get_census(
      const std::vector<uchar>& d_,
      uchar val_,
      std::vector<uchar>& census_
)
{
   int len = (int)d_.size();
   CV_Assert(len%8 == 0);

   int i = 0;
   for (int k = 0; k < len; k += 8)
   {
      get_census_8(d_.data()+k, val_, census_[i]);
      i++;
   }
}

static void
compute_complete_census(
      const std::vector<uchar>& data_,
      std::vector<std::vector<uchar> >& census_
)
{

   int len = (int)data_.size();
   CV_Assert((len-1)%8 == 0);

   std::vector<uchar> tmp((size_t)len-1);

   for (int i = 0; i < len; i++)
   {
      int index = 0;
      for (int k = 0; k < len; k++)
      {
         if (k == i) continue;
         tmp[index] = data_[k];
         index++;
      }

      get_census(tmp, data_[i], census_[i]);
   }
}

static void
complete_census_transform_sequential(
      const cv::Mat& in_image_,
      cv::Mat& census_image_,
      int wnd_size_
)
{
   int n = wnd_size_*wnd_size_ - 1;
   n = n * wnd_size_ * wnd_size_ / 8;

   CV_Assert(in_image_.type() == CV_8UC1);
   CV_Assert(census_image_.size() == in_image_.size());
   CV_Assert(census_image_.depth() == CV_8U);
   CV_Assert(census_image_.channels() == n);
   CV_Assert(wnd_size_&1);
   CV_Assert(wnd_size_ < 16);

   int b = wnd_size_ / 2;
   int ny = in_image_.rows;
   int nx = in_image_.cols;
   int len = wnd_size_*wnd_size_;

   std::vector<uchar> d((size_t)len);
   std::vector<std::vector<uchar> > census((size_t)len);
   for (int i = 0; i < len; i++)
   {
      census[i].resize((size_t)(len-1)/8);
   }

   for (int y = b; y < ny - b; y++)
   {
      for (int x = b; x < nx - b; x++)
      {
         int k = 0;
         for (int dy = -b; dy <= b; dy++)
         {
            for (int dx = -b; dx <= b; dx++)
            {
               d[k] = in_image_.at<uchar>(y+dy, x+dx);
               k++;
            }
         }
         CV_Assert(k == len);

         compute_complete_census(d, census);

         uchar *p = census_image_.ptr<uchar>(y,x);
         for (int i = 0; i < len; i++)
         {
            memcpy(p, census[i].data(), census[i].size());
            p += (int)census[i].size();
         }
      }
   }
}

class CompleteCensusTransformLoopBody : public cv::ParallelLoopBody
{
public:
   CompleteCensusTransformLoopBody(
         const cv::Mat& in_image_,
         cv::Mat& compelete_census_,
         int wnd_size_
   )
   {
      CV_Assert(in_image_.type() == CV_8UC1);
      CV_Assert(compelete_census_.size() == in_image_.size());
      CV_Assert(compelete_census_.depth() == CV_8U);
      CV_Assert(wnd_size_&1);
      CV_Assert(wnd_size_ < 16);

      int n = wnd_size_*wnd_size_ - 1;
      n = n * wnd_size_ * wnd_size_ / 8;
      CV_Assert(compelete_census_.channels() == n);

      m_in_image = in_image_;
      m_complete_census = compelete_census_;
      m_wnd_size = wnd_size_;
   }

   virtual void operator()(const cv::Range& range) const
   {
      int n = m_wnd_size*m_wnd_size - 1;
      n = n * m_wnd_size * m_wnd_size / 8;

      int b = m_wnd_size / 2;
      int nx = m_in_image.cols;
      int len = m_wnd_size*m_wnd_size;

      cv::Mat complete_census = m_complete_census;

      std::vector<uchar> d((size_t)len);
      std::vector<std::vector<uchar> > census((size_t)len);
      for (int i = 0; i < len; i++)
      {
         census[i].resize((size_t)(len-1)/8);
      }

      for (int y = range.start; y < range.end; y++)
      {
         for (int x = b; x < nx - b; x++)
         {
            int k = 0;
            for (int dy = -b; dy <= b; dy++)
            {
               for (int dx = -b; dx <= b; dx++)
               {
                  d[k] = m_in_image.at<uchar>(y+dy, x+dx);
                  k++;
               }
            }
            CV_Assert(k == len);

            compute_complete_census(d, census);

            uchar *p = complete_census.ptr<uchar>(y,x);
            for (int i = 0; i < len; i++)
            {
               memcpy(p, census[i].data(), census[i].size());
               p += (int)census[i].size();
            }
         }
      }

   }

private:
   cv::Mat m_in_image;
   cv::Mat m_complete_census;
   int m_wnd_size;
};

static void
complete_census_transform_gray(
      const cv::Mat& in_image_,
      cv::Mat& census_image_,
      int wnd_size_
)
{
   CV_Assert(in_image_.channels() == 1);
   CV_Assert(wnd_size_&1);
   CV_Assert(wnd_size_ < 16);

   int b = wnd_size_ / 2;

   cv::Mat in_image;
   cv::copyMakeBorder(in_image_, in_image, b, b, b, b, cv::BORDER_REFLECT_101);

   if (in_image.depth() != CV_8U)
   {
      in_image.convertTo(in_image, CV_8U);
   }

   cv::Mat census_image;
   int n = wnd_size_*wnd_size_ - 1;
   n = n * wnd_size_ * wnd_size_ / 8;
   census_image.create(in_image.size(), CV_8UC(n));

#if 0
   complete_census_transform_sequential(in_image, census_image, wnd_size_);
#else
   (void)complete_census_transform_sequential; // remove warning: unused function
   int start_row = b;
   int end_row = in_image.rows - b;
   CompleteCensusTransformLoopBody loop_body(in_image, census_image, wnd_size_);
   cv::parallel_for_(cv::Range(start_row, end_row), loop_body);
#endif

   census_image_ = census_image(cv::Range(b, in_image_.rows+b),
                                cv::Range(b, in_image_.cols+b)).clone();
}

void
complete_census_transform(
      const cv::Mat& in_image_,
      cv::Mat& census_image_,
      int wnd_size_,
      bool color_to_gray_
)
{
   CV_Assert((in_image_.depth() == CV_32F) ||(in_image_.depth() == CV_8U));
   CV_Assert((in_image_.channels() == 1) ||(in_image_.channels() == 3));
   CV_Assert(wnd_size_&1);
   CV_Assert(wnd_size_ < 16);

   if (in_image_.channels() == 1)
   {
      complete_census_transform_gray(in_image_, census_image_, wnd_size_);
   }
   else
   {
      if (color_to_gray_)
      {
         cv::Mat in_image;
         if (in_image_.depth() == CV_32F)
         {
            in_image_.convertTo(in_image, CV_8U);
            cv::cvtColor(in_image, in_image, cv::COLOR_BGR2GRAY);
         }
         else
         {
            cv::cvtColor(in_image_, in_image, cv::COLOR_BGR2GRAY);
         }

         complete_census_transform_gray(in_image, census_image_, wnd_size_);
      }
      else
      {
         cv::Mat bgr[3];
         cv::split(in_image_, bgr);

         cv::Mat census_image[3];
         complete_census_transform_gray(bgr[0], census_image[0], wnd_size_);
         complete_census_transform_gray(bgr[1], census_image[1], wnd_size_);
         complete_census_transform_gray(bgr[2], census_image[2], wnd_size_);
         cv::merge(census_image, 3, census_image_);
      }
   }

   int n = wnd_size_*wnd_size_ - 1;
   n = n * wnd_size_ * wnd_size_ / 8;
   CV_Assert(census_image_.channels() == n);
}
