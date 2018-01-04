#include <iostream>
#include <gtest/gtest.h>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgcodecs.hpp>

#include "RankTransform.hpp"
#include "common.hpp"

TEST(test_RankTransform, test_rank_transform_8u)
{
   uchar d[] = {
        1,  20,  3,  5,  6,
        10, 9,  11,  3,  1,
        0,  5,  22,  30, 4,
        20, 1,  33,  9,  0,
        1,  2,  10,  8,  23
   };

   cv::Mat m(5, 5, CV_8UC1, d);
   cv::Mat rank;

   // window size 3x3
   rank_transform(m, rank, 3);

   uchar a00 = rank.at<uchar>(0,0);
   uchar a01 = rank.at<uchar>(0,1);
   uchar a02 = rank.at<uchar>(0,2);
   uchar a03 = rank.at<uchar>(0,3);
   uchar a04 = rank.at<uchar>(0,4);

   EXPECT_EQ(a00, 0);
   EXPECT_EQ(a01, 8);
   EXPECT_EQ(a02, 0);
   EXPECT_EQ(a03, 5);
   EXPECT_EQ(a04, 8);

   uchar a10 = rank.at<uchar>(1,0);
   uchar a11 = rank.at<uchar>(1,1);
   uchar a12 = rank.at<uchar>(1,2);
   uchar a13 = rank.at<uchar>(1,3);
   uchar a14 = rank.at<uchar>(1,4);
   EXPECT_EQ(a10, 6);
   EXPECT_EQ(a11, 4);
   EXPECT_EQ(a12, 5);
   EXPECT_EQ(a13, 1);
   EXPECT_EQ(a14, 0);

   uchar a20 = rank.at<uchar>(2,0);
   uchar a21 = rank.at<uchar>(2,1);
   uchar a22 = rank.at<uchar>(2,2);
   uchar a23 = rank.at<uchar>(2,3);
   uchar a24 = rank.at<uchar>(2,4);
   EXPECT_EQ(a20, 0);
   EXPECT_EQ(a21, 2);
   EXPECT_EQ(a22, 6);
   EXPECT_EQ(a23, 7);
   EXPECT_EQ(a24, 4);

   uchar a30 = rank.at<uchar>(3,0);
   uchar a31 = rank.at<uchar>(3,1);
   uchar a32 = rank.at<uchar>(3,2);
   uchar a33 = rank.at<uchar>(3,3);
   uchar a34 = rank.at<uchar>(3,4);
   EXPECT_EQ(a30, 8);
   EXPECT_EQ(a31, 1);
   EXPECT_EQ(a32, 8);
   EXPECT_EQ(a33, 3);
   EXPECT_EQ(a34, 0);

   uchar a40 = rank.at<uchar>(4,0);
   uchar a41 = rank.at<uchar>(4,1);
   uchar a42 = rank.at<uchar>(4,2);
   uchar a43 = rank.at<uchar>(4,3);
   uchar a44 = rank.at<uchar>(4,4);
   EXPECT_EQ(a40, 0);
   EXPECT_EQ(a41, 3);
   EXPECT_EQ(a42, 6);
   EXPECT_EQ(a43, 2);
   EXPECT_EQ(a44, 8);

   // window size 5x5
   rank_transform(m, rank, 5);

   a00 = rank.at<uchar>(0,0);
   a01 = rank.at<uchar>(0,1);
   a02 = rank.at<uchar>(0,2);
   a03 = rank.at<uchar>(0,3);
   a04 = rank.at<uchar>(0,4);
   EXPECT_EQ(a00, 2);
   EXPECT_EQ(a01, 19);
   EXPECT_EQ(a02, 5);
   EXPECT_EQ(a03, 9);
   EXPECT_EQ(a04, 12);

   a10 = rank.at<uchar>(1,0);
   a11 = rank.at<uchar>(1,1);
   a12 = rank.at<uchar>(1,2);
   a13 = rank.at<uchar>(1,3);
   a14 = rank.at<uchar>(1,4);
   EXPECT_EQ(a10, 12);
   EXPECT_EQ(a11, 10);
   EXPECT_EQ(a12, 18);
   EXPECT_EQ(a13, 4);
   EXPECT_EQ(a14, 1);

   a20 = rank.at<uchar>(2,0);
   a21 = rank.at<uchar>(2,1);
   a22 = rank.at<uchar>(2,2);
   a23 = rank.at<uchar>(2,3);
   a24 = rank.at<uchar>(2,4);
   EXPECT_EQ(a20, 0);
   EXPECT_EQ(a21, 9);
   EXPECT_EQ(a22, 21);
   EXPECT_EQ(a23, 22);
   EXPECT_EQ(a24, 6);

   a30 = rank.at<uchar>(3,0);
   a31 = rank.at<uchar>(3,1);
   a32 = rank.at<uchar>(3,2);
   a33 = rank.at<uchar>(3,3);
   a34 = rank.at<uchar>(3,4);
   EXPECT_EQ(a30, 17);
   EXPECT_EQ(a31, 1);
   EXPECT_EQ(a32, 23);
   EXPECT_EQ(a33, 12);
   EXPECT_EQ(a34, 0);

   a40 = rank.at<uchar>(4,0);
   a41 = rank.at<uchar>(4,1);
   a42 = rank.at<uchar>(4,2);
   a43 = rank.at<uchar>(4,3);
   a44 = rank.at<uchar>(4,4);
   EXPECT_EQ(a40, 2);
   EXPECT_EQ(a41, 7);
   EXPECT_EQ(a42, 15);
   EXPECT_EQ(a43, 9);
   EXPECT_EQ(a44, 16);
}

TEST(test_RankTransform, test_rank_transform_image_8u)
{
   cv::String filename = cv::String(KFJ_DATA_PATH) + "/teddy/frame10.png";
   cv::Mat img = cv::imread(filename, cv::IMREAD_GRAYSCALE);
   if (img.empty())
   {
      std::cout << "Cannot read file: '" << filename << "'." << std::endl;
      return;
   }
   cv::Mat rank;
   cv::TickMeter tm;
   tm.start();
   rank_transform(img, rank, 11);
   tm.stop();
   std::cout << "rank_transform took " << tm.getTimeSec() << " s" << std::endl;

   rank.convertTo(rank, CV_32F);
   cv::normalize(rank, rank, 1, 0, cv::NORM_MINMAX);
   rank = rank*255;
   rank.convertTo(rank, CV_8U);

   cv::imwrite("teddy-rank-11x11-scaled-to-255.png", rank);
#if 0
   cv::namedWindow("rank", CV_WINDOW_NORMAL);
   cv::imshow("rank", rank + 30 );
   cv::waitKey(0);
#endif
}
