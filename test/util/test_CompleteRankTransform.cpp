#include <iostream>
#include <gtest/gtest.h>
#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>

#include "CompleteRankTransform.hpp"
#include "common.hpp"

TEST(test_CompleteRankTransform, test_complete_rank_transform_8u)
{
   uchar d[] = {
        1,  20,  3,  5,  6,
        10, 9,  11,  3,  1,
        0,  5,  22,  30, 4,
        20, 1,  33,  9,  0,
        1,  2,  10,  8,  23
   };

   cv::Mat m(5, 5, CV_8UC1, d);
   cv::Mat complete_rank;

   // window size 3x3
   complete_rank_transform(m, complete_rank, 3, true);
   CV_Assert(complete_rank.channels() == 9);
   CV_Assert(complete_rank.depth() == CV_8U);

   uchar *p = complete_rank.ptr<uchar>(0,0);
   EXPECT_EQ(p[0], 1);
   EXPECT_EQ(p[1], 5);
   EXPECT_EQ(p[2], 1);
   EXPECT_EQ(p[3], 7);
   EXPECT_EQ(p[4], 0);
   EXPECT_EQ(p[5], 7);
   EXPECT_EQ(p[6], 1);
   EXPECT_EQ(p[7], 5);
   EXPECT_EQ(p[8], 1);

   p = complete_rank.ptr<uchar>(0,1);
   EXPECT_EQ(p[0], 4);
   EXPECT_EQ(p[1], 2);
   EXPECT_EQ(p[2], 6);
   EXPECT_EQ(p[3], 0);
   EXPECT_EQ(p[4], 8);
   EXPECT_EQ(p[5], 1);
   EXPECT_EQ(p[6], 4);
   EXPECT_EQ(p[7], 2);
   EXPECT_EQ(p[8], 6);

   p = complete_rank.ptr<uchar>(1,3);
   EXPECT_EQ(p[0], 1);
   EXPECT_EQ(p[1], 4);
   EXPECT_EQ(p[2], 5);
   EXPECT_EQ(p[3], 6);
   EXPECT_EQ(p[4], 1);
   EXPECT_EQ(p[5], 0);
   EXPECT_EQ(p[6], 7);
   EXPECT_EQ(p[7], 8);
   EXPECT_EQ(p[8], 3);

   p = complete_rank.ptr<uchar>(2,2);
   EXPECT_EQ(p[0], 3);
   EXPECT_EQ(p[1], 5);
   EXPECT_EQ(p[2], 1);
   EXPECT_EQ(p[3], 2);
   EXPECT_EQ(p[4], 6);
   EXPECT_EQ(p[5], 7);
   EXPECT_EQ(p[6], 0);
   EXPECT_EQ(p[7], 8);
   EXPECT_EQ(p[8], 3);

   p = complete_rank.ptr<uchar>(3,1);
   EXPECT_EQ(p[0], 0);
   EXPECT_EQ(p[1], 4);
   EXPECT_EQ(p[2], 7);
   EXPECT_EQ(p[3], 6);
   EXPECT_EQ(p[4], 1);
   EXPECT_EQ(p[5], 8);
   EXPECT_EQ(p[6], 1);
   EXPECT_EQ(p[7], 3);
   EXPECT_EQ(p[8], 5);

   p = complete_rank.ptr<uchar>(4,3);
   EXPECT_EQ(p[0], 7);
   EXPECT_EQ(p[1], 3);
   EXPECT_EQ(p[2], 0);
   EXPECT_EQ(p[3], 5);
   EXPECT_EQ(p[4], 2);
   EXPECT_EQ(p[5], 6);
   EXPECT_EQ(p[6], 7);
   EXPECT_EQ(p[7], 3);
   EXPECT_EQ(p[8], 0);

   // window size 5x5
   complete_rank_transform(m, complete_rank, 5, true);
   CV_Assert(complete_rank.channels() == 25);
   CV_Assert(complete_rank.depth() == CV_8U);

   p = complete_rank.ptr<uchar>(2,2);
   EXPECT_EQ(p[0], 2);
   EXPECT_EQ(p[1], 19);
   EXPECT_EQ(p[2], 7);
   EXPECT_EQ(p[3], 10);
   EXPECT_EQ(p[4], 12);
   EXPECT_EQ(p[5], 16);
   EXPECT_EQ(p[6], 14);
   EXPECT_EQ(p[7], 18);
   EXPECT_EQ(p[8], 7);
   EXPECT_EQ(p[9], 2);
   EXPECT_EQ(p[10],0);
   EXPECT_EQ(p[11],10);
   EXPECT_EQ(p[12],21);
   EXPECT_EQ(p[13],23);
   EXPECT_EQ(p[14],9);
   EXPECT_EQ(p[15],19);
   EXPECT_EQ(p[16],2);
   EXPECT_EQ(p[17],24);
   EXPECT_EQ(p[18],14);
   EXPECT_EQ(p[19],0);
   EXPECT_EQ(p[20],2);
   EXPECT_EQ(p[21],6);
   EXPECT_EQ(p[22],16);
   EXPECT_EQ(p[23],13);
   EXPECT_EQ(p[24],22);

   p = complete_rank.ptr<uchar>(3,3);
   EXPECT_EQ(p[0],12);
   EXPECT_EQ(p[1],18);
}

TEST(test_CompleteRankTransform, test_complete_rank_transform_image_8u)
{
   cv::String filename = cv::String(KFJ_DATA_PATH) + "/teddy/frame10.png";

   cv::Mat img = cv::imread(filename, cv::IMREAD_GRAYSCALE);
   if (img.empty())
   {
      std::cout << "Cannot read file: '" << filename << "'." << std::endl;
      return;
   }
   cv::Mat complete_rank;
   cv::TickMeter tm;
   tm.start();
   complete_rank_transform(img, complete_rank, 5, true);
   tm.stop();
   std::cout << "complete_rank_transform took " << tm.getTimeSec() << " s" << std::endl;
}
