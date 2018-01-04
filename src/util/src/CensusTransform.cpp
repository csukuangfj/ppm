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

static uchar
get_census_8(const uchar *data_, uchar val_)
{
   uchar res = 0;
   for (int i = 0; i < 8; i++)
   {
      if (data_[i] < val_)
      {
         res |= 1;
      }

      if (i != 7)
      {
         res <<= 1;
      }
   }
   return res;
}

static cv::Vec3b
get_census_24(const uchar *data_, uchar val_)
{
   cv::Vec3b res;
   res[0] = get_census_8(data_, val_);
   res[1] = get_census_8(data_ + 8, val_);
   res[2] = get_census_8(data_ + 16, val_);
   return res;
}

/**
 * Non-parallel version.
 * @param in_image_       [in] CV_8UC1
 * @param census_image_   [out] CV_8UC1 or CV_8UC3
 * @param wnd_size_       [in] Neighborhood size, 3 or 5.
 */
static void
census_transform_(const cv::Mat& in_image_, cv::Mat& census_image_, int wnd_size_)
{
   CV_Assert(in_image_.type() == CV_8UC1);
   CV_Assert((wnd_size_== 3) || (wnd_size_ == 5));
   CV_Assert((census_image_.type() == CV_8UC1) ||(census_image_.type() == CV_8UC3));
   CV_Assert(in_image_.size() == census_image_.size());

   int ny = in_image_.rows;
   int nx = in_image_.cols;

   int r = wnd_size_ / 2;
   int len = wnd_size_*wnd_size_-1;

   std::vector<uchar> d((size_t)len);
   for (int y = r; y < (ny - r); y++)
   {
      const uchar *pval = in_image_.ptr<uchar>(y);
      for (int x = r; x < (nx -r); x++)
      {
         int k = 0;
         for (int j = -r; j <= r; j++)
         {
            int jj = y + j;
            const uchar *p = in_image_.ptr<uchar>(jj);
            for (int i = -r; i <= r; i++)
            {
               int ii = x + i;
               if ((y == jj) && (x == ii)) continue;
               d[k] = p[ii];
               k++;
            }
         }

         if (wnd_size_ == 3)
         {
            census_image_.at<uchar>(y,x) = get_census_8(d.data(), pval[x]);
         }
         else
         {
            census_image_.at<cv::Vec3b>(y,x) = get_census_24(d.data(), pval[x]);
         }
      }
   }
}

class CensusTransformLoopBody : public cv::ParallelLoopBody
{
public:
   CensusTransformLoopBody(const cv::Mat& in_image_,
                           cv::Mat& census_image_,
                           int wnd_size_)
   {
      CV_Assert(in_image_.type() == CV_8UC1);
      CV_Assert((wnd_size_== 3) || (wnd_size_ == 5));
      CV_Assert((census_image_.type() == CV_8UC1) ||(census_image_.type() == CV_8UC3));
      CV_Assert(in_image_.size() == census_image_.size());

      m_in_image = in_image_;
      m_census_image = census_image_;
      m_wnd_size = wnd_size_;
   }

   virtual void operator ()(const cv::Range& range) const
   {
      int r = m_wnd_size / 2;
      int nx = m_in_image.cols;
      int len = m_wnd_size*m_wnd_size-1;
      std::vector<uchar> d((size_t)len);

      // the member of this class is non-modifiable.
      // thus it needs to get an alias to m_census_image
      cv::Mat census_image = m_census_image;

      for (int y = range.start; y < range.end; y++)
      {
         const uchar *pval = m_in_image.ptr<uchar>(y);
         for (int x = r; x < nx - r; x++)
         {
            int k = 0;
            for (int j = -r; j <= r; j++)
            {
               int jj = y + j;
               const uchar *p = m_in_image.ptr<uchar>(jj);
               for (int i = -r; i <= r; i++)
               {
                  int ii = x + i;
                  if ((y == jj) && (x == ii)) continue;
                  d[k] = p[ii];
                  k++;
               }
            }

            if (m_wnd_size == 3)
            {
               census_image.at<uchar>(y, x) = get_census_8(d.data(), pval[x]);
            }
            else
            {
               // wnd_size is 5
               census_image.at<cv::Vec3b>(y, x) = get_census_24(d.data(), pval[x]);
            }
         }
      }
   }

private:
   cv::Mat m_in_image;     //!< input image, CV_8UC1
   cv::Mat m_census_image; //!< output census image, CV_8UC1 or CV_8UC3
   int m_wnd_size;         //!< 3 or 5
};

static void
census_transform_gray(const cv::Mat& in_image_,
                      cv::Mat& census_image_,
                      int wnd_size_)
{
   CV_Assert(in_image_.channels() == 1);
   CV_Assert((wnd_size_== 3) || (wnd_size_ == 5));

   int b = wnd_size_ / 2;

   cv::Mat in_image;
   cv::copyMakeBorder(in_image_, in_image, b, b, b, b, cv::BORDER_REFLECT_101);

   if (in_image.depth() != CV_8U)
   {
      in_image.convertTo(in_image, CV_8U);
   }

   cv::Mat census_image;

   if (wnd_size_ == 3)
   {
      census_image.create(in_image.size(), CV_8UC1); // wnd_size == 3, 8 bits, 1 byte
   }
   else
   {
      census_image.create(in_image.size(), CV_8UC3); // wnd_size == 5, 24 bits, 3 bytes
   }

   int start_row = b;
   int end_row = in_image.rows - b;
   CensusTransformLoopBody loop_body(in_image, census_image, wnd_size_);
   cv::parallel_for_(cv::Range(start_row, end_row), loop_body);

   census_image_ = census_image(cv::Range(b, b+in_image_.rows),
                                cv::Range(b, b+in_image_.cols)).clone();
}

void
census_transform(
      const cv::Mat& in_image_,
      cv::Mat& census_image_,
      int wnd_size_,
      bool color_to_gray_
)
{
   (void)census_transform_; // remove warnings
   CV_Assert((in_image_.depth() == CV_32F) ||(in_image_.depth() == CV_8U));
   CV_Assert((in_image_.channels() == 1) ||(in_image_.channels() == 3));

   if (in_image_.channels() == 1)
   {
      census_transform_gray(in_image_, census_image_, wnd_size_);
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

         census_transform_gray(in_image, census_image_, wnd_size_);
      }
      else
      {
         cv::Mat bgr[3];
         cv::split(in_image_, bgr);

         cv::Mat census_image[3];
         census_transform_gray(bgr[0], census_image[0], wnd_size_);
         census_transform_gray(bgr[1], census_image[1], wnd_size_);
         census_transform_gray(bgr[2], census_image[2], wnd_size_);

         cv::merge(census_image, 3, census_image_);
         if (wnd_size_ == 3)
         {
            CV_Assert(census_image_.channels() == 3);
         }
         else
         {
            CV_Assert(census_image_.channels() == 9);
         }
      }
   }
}
