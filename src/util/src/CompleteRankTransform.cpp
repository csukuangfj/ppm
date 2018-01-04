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
get_rank(
      const std::vector<uchar>& data_,
      uchar val_,
      uchar& rank_
)
{
   rank_ = 0;
   int len = (int)data_.size();
   for (int i = 0; i < len; i++)
   {
      if (data_[i] < val_)
      {
         rank_++;
      }
   }
}

/**
 *
 * @param data_   [in]
 * @param rank_   [out]
 */
static void
compute_complete_rank(
      const std::vector<uchar>& data_,
      std::vector<uchar>& rank_
)
{
   CV_Assert(rank_.size() == data_.size());

   int len = (int)data_.size();
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

      get_rank(tmp, data_[i], rank_[i]);
   }
}

static void
complete_rank_transform_sequential(
      const cv::Mat& in_image_,
      cv::Mat& rank_image_,
      int wnd_size_
)
{
   CV_Assert(in_image_.type() == CV_8UC1);
   CV_Assert(rank_image_.size() == in_image_.size());
   CV_Assert(rank_image_.depth() == CV_8U);
   CV_Assert(rank_image_.channels() == wnd_size_*wnd_size_);
   CV_Assert(wnd_size_&1);
   CV_Assert(wnd_size_ < 16);

   int b = wnd_size_ / 2;
   int ny = in_image_.rows;
   int nx = in_image_.cols;
   int len = wnd_size_*wnd_size_;
   std::vector<uchar> d((size_t)len);
   std::vector<uchar> rank((size_t)len);

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

         compute_complete_rank(d, rank);

         uchar *p = rank_image_.ptr<uchar>(y,x);
         memcpy(p, rank.data(), (size_t)len);
      }
   }
}

class CompleteRankTransformLoopBody : public cv::ParallelLoopBody
{
public:
   CompleteRankTransformLoopBody(
         const cv::Mat& in_image_,
         cv::Mat& compelete_rank_,
         int wnd_size_
   )
   {
      CV_Assert(in_image_.type() == CV_8UC1);
      CV_Assert(compelete_rank_.size() == in_image_.size());
      CV_Assert(compelete_rank_.depth() == CV_8U);
      CV_Assert(compelete_rank_.channels() == wnd_size_*wnd_size_);
      CV_Assert(wnd_size_&1);
      CV_Assert(wnd_size_ < 16);

      m_in_image = in_image_;
      m_complete_rank = compelete_rank_;
      m_wnd_size = wnd_size_;
   }

   virtual void operator()(const cv::Range& range) const
   {
      int b = m_wnd_size / 2;
      int nx = m_in_image.cols;
      int len = m_wnd_size*m_wnd_size;
      std::vector<uchar> d((size_t)len);
      std::vector<uchar> rank((size_t)len);

      cv::Mat complete_rank = m_complete_rank;

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

            compute_complete_rank(d, rank);

            uchar *p = complete_rank.ptr<uchar>(y,x);
            memcpy(p, rank.data(), (size_t)len);
         }
      }
   }

private:
   cv::Mat m_in_image;
   cv::Mat m_complete_rank;
   int m_wnd_size;
};

static void
complete_rank_transform_gray(
      const cv::Mat& in_image_,
      cv::Mat& rank_image_,
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

   cv::Mat rank_image;
   int n = wnd_size_*wnd_size_;
   rank_image.create(in_image.size(), CV_8UC(n));

#if 0
   complete_rank_transform_sequential(in_image, rank_image, wnd_size_);
#else
   (void)complete_rank_transform_sequential; // remove warning: unused function
   int start_row = b;
   int end_row = in_image.rows - b;
   CompleteRankTransformLoopBody loop_body(in_image, rank_image, wnd_size_);
   cv::parallel_for_(cv::Range(start_row, end_row), loop_body);
#endif

   rank_image_ = rank_image(cv::Range(b, in_image_.rows+b),
                            cv::Range(b, in_image_.cols+b)).clone();
}

void
complete_rank_transform(
      const cv::Mat& in_image_,
      cv::Mat& rank_image_,
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
      complete_rank_transform_gray(in_image_, rank_image_, wnd_size_);
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

         complete_rank_transform_gray(in_image, rank_image_, wnd_size_);
      }
      else
      {
         cv::Mat bgr[3];
         cv::split(in_image_, bgr);

         cv::Mat rank_image[3];
         complete_rank_transform_gray(bgr[0], rank_image[0], wnd_size_);
         complete_rank_transform_gray(bgr[1], rank_image[1], wnd_size_);
         complete_rank_transform_gray(bgr[2], rank_image[2], wnd_size_);

         cv::merge(rank_image, 3, rank_image_);
         CV_Assert(rank_image_.channels() == (wnd_size_*wnd_size_));
      }
   }
}
