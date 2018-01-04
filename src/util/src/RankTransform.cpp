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

/**
 * Sequential implementation (slower)
 *
 * @param in_image_         [in] CV_8UC1
 * @param rank_image_       [out] CV_8UC1
 * @param wnd_size_         [in] An odd number
 */
static void
rank_transform_(
      const cv::Mat& in_image_,
      cv::Mat& rank_image_,
      int wnd_size_
)
{
   CV_Assert(in_image_.type() == CV_8UC1);
   CV_Assert(rank_image_.type() == CV_8UC1);
   CV_Assert(in_image_.size() == rank_image_.size());
   CV_Assert(wnd_size_&1);
   CV_Assert(wnd_size_ < 16);

   int ny = in_image_.rows;
   int nx = in_image_.cols;

   int r = wnd_size_ / 2;

   for (int y = r; y < ny - r; y++)
   {
      const uchar *pval = in_image_.ptr<uchar>(y);
      for (int x = r; x < nx - r; x++)
      {
         uchar rank = 0;
         for (int dy = -r; dy <= r; dy++)
         {
            int yy = y + dy;
            for (int dx = -r; dx <= r; dx++)
            {
               uchar neighbor_val = in_image_.at<uchar>(yy, x+dx);
               if (neighbor_val < pval[x])
               {
                  rank++;
               }
            }
         }

         rank_image_.at<uchar>(y,x) = rank;
      }
   }
}

class RankTransformLoopBody : public cv::ParallelLoopBody
{
public:
   RankTransformLoopBody(
         const cv::Mat& in_image_,
         cv::Mat& rank_image_,
         int wnd_size_
   )
   {
      CV_Assert(in_image_.type() == CV_8UC1);
      CV_Assert(rank_image_.type() == CV_8UC1);
      CV_Assert(in_image_.size() == rank_image_.size());
      CV_Assert(wnd_size_&1);
      CV_Assert(wnd_size_ < 16);

      m_in_image = in_image_;
      m_rank_image = rank_image_;
      m_wnd_size = wnd_size_;
   }

   virtual void operator ()(const cv::Range& range) const
   {
      int nx = m_rank_image.cols;
      int r = m_wnd_size / 2;
      cv::Mat rank_image = m_rank_image;
      for (int y = range.start; y < range.end; y++)
      {
         const uchar *pval = m_in_image.ptr<uchar>(y);
         for (int x = r; x < nx - r; x++)
         {
            uchar rank = 0;
            for (int dy = -r; dy <= r; dy++)
            {
               int yy = y + dy;
               for (int dx = -r; dx <= r; dx++)
               {
                  uchar neighbor_val = m_in_image.at<uchar>(yy, x+dx);
                  if (neighbor_val < pval[x])
                  {
                     rank++;
                  }
               }
            }

            rank_image.at<uchar>(y,x) = rank;
         }
      }
   }

private:
   cv::Mat m_in_image;
   cv::Mat m_rank_image;
   int m_wnd_size;
};

static void
rank_transform_gray(
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
   rank_image.create(in_image.size(), CV_8UC1);

   int start_row = b;
   int end_row = in_image.rows - b;
   RankTransformLoopBody loop_body(in_image, rank_image, wnd_size_);
   cv::parallel_for_(cv::Range(start_row, end_row), loop_body);

   rank_image_ = rank_image(cv::Range(b, in_image_.rows+b),
                            cv::Range(b, in_image_.cols+b)).clone();

}

void
rank_transform(
      const cv::Mat& in_image_,
      cv::Mat& rank_image_,
      int wnd_size_,
      bool color_to_gray_
)
{
   (void)rank_transform_; // remove warnings

   CV_Assert((in_image_.depth() == CV_32F) ||(in_image_.depth() == CV_8U));
   CV_Assert((in_image_.channels() == 1) ||(in_image_.channels() == 3));
   CV_Assert(wnd_size_&1);
   CV_Assert(wnd_size_ < 16);

   if (in_image_.channels() == 1)
   {
      rank_transform_gray(in_image_, rank_image_, wnd_size_);
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

         rank_transform_gray(in_image, rank_image_, wnd_size_);
      }
      else
      {
         cv::Mat bgr[3];
         cv::split(in_image_, bgr);

         cv::Mat rank_image[3];
         rank_transform_gray(bgr[0], rank_image[0], wnd_size_);
         rank_transform_gray(bgr[1], rank_image[1], wnd_size_);
         rank_transform_gray(bgr[2], rank_image[2], wnd_size_);

         cv::merge(rank_image, 3, rank_image_);
         CV_Assert(rank_image_.channels() == 3);
      }
   }
}
