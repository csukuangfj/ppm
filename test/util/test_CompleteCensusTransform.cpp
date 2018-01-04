#include <iostream>
#include <gtest/gtest.h>
#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>

#include "CompleteCensusTransform.hpp"
#include "common.hpp"

TEST(test_CompleteCensusTransform, test_complete_census_transform_8u)
{
   uchar d[] = {
        1,  20,  3,  5,  6,
        10, 9,  11,  3,  1,
        0,  5,  22,  30, 4,
        20, 1,  33,  9,  0,
        1,  2,  10,  8,  23
   };

   cv::Mat m(5, 5, CV_8UC1, d);
   cv::Mat complete_census;

   // window size 3x3
   complete_census_transform(m, complete_census, 3, true);
   CV_Assert(complete_census.channels() == 9);
   CV_Assert(complete_census.depth() == CV_8U);

   uchar *p = complete_census.ptr<uchar>(0,0);
   EXPECT_EQ(p[0], 0b00010000);
   EXPECT_EQ(p[1], 0b11010101);
   EXPECT_EQ(p[2], 0b00010000);
   EXPECT_EQ(p[3], 0b11110111);
   EXPECT_EQ(p[4], 0b00000000);
   EXPECT_EQ(p[5], 0b11101111);
   EXPECT_EQ(p[6], 0b00001000);
   EXPECT_EQ(p[7], 0b10101011);
   EXPECT_EQ(p[8], 0b00001000);

   p = complete_census.ptr<uchar>(0,1);
   EXPECT_EQ(p[0], 0b10101010);
   EXPECT_EQ(p[1], 0b00101000);
   EXPECT_EQ(p[2], 0b11101110);
   EXPECT_EQ(p[3], 0b00000000);
   EXPECT_EQ(p[4], 0b11111111);
   EXPECT_EQ(p[5], 0b00010000);
   EXPECT_EQ(p[6], 0b01010110);
   EXPECT_EQ(p[7], 0b00010100);
   EXPECT_EQ(p[8], 0b11010111);

   p = complete_census.ptr<uchar>(0,2);
   EXPECT_EQ(p[0], 0b01011001);
   EXPECT_EQ(p[1], 0b11011101);
   EXPECT_EQ(p[2], 0b00000000);
   EXPECT_EQ(p[3], 0b11111111);
   EXPECT_EQ(p[4], 0b00000000);
   EXPECT_EQ(p[5], 0b00101001);
   EXPECT_EQ(p[6], 0b00101101);
   EXPECT_EQ(p[7], 0b10101111);
   EXPECT_EQ(p[8], 0b00000000);

   p = complete_census.ptr<uchar>(0,3);
   EXPECT_EQ(p[0], 0b11111011);
   EXPECT_EQ(p[1], 0b01000001);
   EXPECT_EQ(p[2], 0b00000000);
   EXPECT_EQ(p[3], 0b00100001);
   EXPECT_EQ(p[4], 0b01110011);
   EXPECT_EQ(p[5], 0b01111011);
   EXPECT_EQ(p[6], 0b01111111);
   EXPECT_EQ(p[7], 0b00100001);
   EXPECT_EQ(p[8], 0b00000000);

   p = complete_census.ptr<uchar>(0,4);
   EXPECT_EQ(p[0], 0b10000010);
   EXPECT_EQ(p[1], 0b00000000);
   EXPECT_EQ(p[2], 0b01000010);
   EXPECT_EQ(p[3], 0b11100111);
   EXPECT_EQ(p[4], 0b11111111);
   EXPECT_EQ(p[5], 0b11100111);
   EXPECT_EQ(p[6], 0b01000010);
   EXPECT_EQ(p[7], 0b00000000);
   EXPECT_EQ(p[8], 0b01000001);

   p = complete_census.ptr<uchar>(1,0);
   EXPECT_EQ(p[0], 0b10111111);
   EXPECT_EQ(p[1], 0b00000010);
   EXPECT_EQ(p[2], 0b01111111);
   EXPECT_EQ(p[3], 0b01000111);
   EXPECT_EQ(p[4], 0b01011111);
   EXPECT_EQ(p[5], 0b01000111);
   EXPECT_EQ(p[6], 0b01000010);
   EXPECT_EQ(p[7], 0b00000000);
   EXPECT_EQ(p[8], 0b01000001);

   p = complete_census.ptr<uchar>(1,1);
   EXPECT_EQ(p[0], 0b00000100);
   EXPECT_EQ(p[1], 0b11111110);
   EXPECT_EQ(p[2], 0b10000100);
   EXPECT_EQ(p[3], 0b10110110);
   EXPECT_EQ(p[4], 0b10100110);
   EXPECT_EQ(p[5], 0b10111110);
   EXPECT_EQ(p[6], 0b00000000);
   EXPECT_EQ(p[7], 0b10100010);
   EXPECT_EQ(p[8], 0b11111111);


   p = complete_census.ptr<uchar>(1,2);
   EXPECT_EQ(p[0], 0b11111100);
   EXPECT_EQ(p[1], 0b00000000);
   EXPECT_EQ(p[2], 0b01001000);
   EXPECT_EQ(p[3], 0b01101100);
   EXPECT_EQ(p[4], 0b01111100);
   EXPECT_EQ(p[5], 0b00000000);
   EXPECT_EQ(p[6], 0b01000100);
   EXPECT_EQ(p[7], 0b11111110);
   EXPECT_EQ(p[8], 0b11111111);

   p = complete_census.ptr<uchar>(1,3);
   EXPECT_EQ(p[0], 0b00001000);
   EXPECT_EQ(p[1], 0b10011001);
   EXPECT_EQ(p[2], 0b11011001);
   EXPECT_EQ(p[3], 0b11111001);
   EXPECT_EQ(p[4], 0b00001000);
   EXPECT_EQ(p[5], 0b00000000);
   EXPECT_EQ(p[6], 0b11111101);
   EXPECT_EQ(p[7], 0b11111111);
   EXPECT_EQ(p[8], 0b10001100);

   p = complete_census.ptr<uchar>(1,4);
   EXPECT_EQ(p[0], 0b00111010);
   EXPECT_EQ(p[1], 0b11111010);
   EXPECT_EQ(p[2], 0b00111010);
   EXPECT_EQ(p[3], 0b00010000);
   EXPECT_EQ(p[4], 0b00000000);
   EXPECT_EQ(p[5], 0b00001000);
   EXPECT_EQ(p[6], 0b11111110);
   EXPECT_EQ(p[7], 0b00011100);
   EXPECT_EQ(p[8], 0b11111101);

   p = complete_census.ptr<uchar>(2,0);
   EXPECT_EQ(p[0], 0b00111101);
   EXPECT_EQ(p[1], 0b11111101);
   EXPECT_EQ(p[2], 0b00111101);
   EXPECT_EQ(p[3], 0b00010101);
   EXPECT_EQ(p[4], 0b00000000);
   EXPECT_EQ(p[5], 0b00001101);
   EXPECT_EQ(p[6], 0b00001000);
   EXPECT_EQ(p[7], 0b11111111);
   EXPECT_EQ(p[8], 0b00001000);

   p = complete_census.ptr<uchar>(2,1);
   EXPECT_EQ(p[0], 0b10110010);
   EXPECT_EQ(p[1], 0b00110010);
   EXPECT_EQ(p[2], 0b11110010);
   EXPECT_EQ(p[3], 0b00000000);
   EXPECT_EQ(p[4], 0b00010010);
   EXPECT_EQ(p[5], 0b11111110);
   EXPECT_EQ(p[6], 0b11111010);
   EXPECT_EQ(p[7], 0b00010000);
   EXPECT_EQ(p[8], 0b11111111);

   p = complete_census.ptr<uchar>(2,2);
   EXPECT_EQ(p[0], 0b01100100);
   EXPECT_EQ(p[1], 0b11100101);
   EXPECT_EQ(p[2], 0b00000100);
   EXPECT_EQ(p[3], 0b00100100);
   EXPECT_EQ(p[4], 0b11110101);
   EXPECT_EQ(p[5], 0b11111101);
   EXPECT_EQ(p[6], 0b00000000);
   EXPECT_EQ(p[7], 0b11111111);
   EXPECT_EQ(p[8], 0b00110010);

   p = complete_census.ptr<uchar>(2,3);
   EXPECT_EQ(p[0], 0b11001011);
   EXPECT_EQ(p[1], 0b01000001);
   EXPECT_EQ(p[2], 0b00000001);
   EXPECT_EQ(p[3], 0b11101011);
   EXPECT_EQ(p[4], 0b11111011);
   EXPECT_EQ(p[5], 0b01100001);
   EXPECT_EQ(p[6], 0b11111111);
   EXPECT_EQ(p[7], 0b01100101);
   EXPECT_EQ(p[8], 0b00000000);

   p = complete_census.ptr<uchar>(2,4);
   EXPECT_EQ(p[0], 0b10000010);
   EXPECT_EQ(p[1], 0b00000010);
   EXPECT_EQ(p[2], 0b01000010);
   EXPECT_EQ(p[3], 0b11110111);
   EXPECT_EQ(p[4], 0b11100010);
   EXPECT_EQ(p[5], 0b11101111);
   EXPECT_EQ(p[6], 0b11101010);
   EXPECT_EQ(p[7], 0b00000000);
   EXPECT_EQ(p[8], 0b11101001);

   p = complete_census.ptr<uchar>(3,0);
   EXPECT_EQ(p[0], 0b10101111);
   EXPECT_EQ(p[1], 0b00000000);
   EXPECT_EQ(p[2], 0b01101111);
   EXPECT_EQ(p[3], 0b01000000);
   EXPECT_EQ(p[4], 0b11111111);
   EXPECT_EQ(p[5], 0b01000000);
   EXPECT_EQ(p[6], 0b01010110);
   EXPECT_EQ(p[7], 0b01000000);
   EXPECT_EQ(p[8], 0b01010101);

   p = complete_census.ptr<uchar>(3,1);
   EXPECT_EQ(p[0], 0b00000000);
   EXPECT_EQ(p[1], 0b10010110);
   EXPECT_EQ(p[2], 0b11110111);
   EXPECT_EQ(p[3], 0b11010111);
   EXPECT_EQ(p[4], 0b10000000);
   EXPECT_EQ(p[5], 0b11111111);
   EXPECT_EQ(p[6], 0b10000000);
   EXPECT_EQ(p[7], 0b10001010);
   EXPECT_EQ(p[8], 0b11001011);

   p = complete_census.ptr<uchar>(3,2);
   EXPECT_EQ(p[0], 0b00100100);
   EXPECT_EQ(p[1], 0b10101111);
   EXPECT_EQ(p[2], 0b11101111);
   EXPECT_EQ(p[3], 0b00000000);
   EXPECT_EQ(p[4], 0b11111111);
   EXPECT_EQ(p[5], 0b10010101);
   EXPECT_EQ(p[6], 0b00010000);
   EXPECT_EQ(p[7], 0b10010111);
   EXPECT_EQ(p[8], 0b10010010);

   p = complete_census.ptr<uchar>(3,3);
   EXPECT_EQ(p[0], 0b01011110);
   EXPECT_EQ(p[1], 0b11011111);
   EXPECT_EQ(p[2], 0b00001000);
   EXPECT_EQ(p[3], 0b11111111);
   EXPECT_EQ(p[4], 0b00101010);
   EXPECT_EQ(p[5], 0b00000000);
   EXPECT_EQ(p[6], 0b00101110);
   EXPECT_EQ(p[7], 0b00100100);
   EXPECT_EQ(p[8], 0b10101111);

   p = complete_census.ptr<uchar>(3,4);
   EXPECT_EQ(p[0], 0b10111111);
   EXPECT_EQ(p[1], 0b00010000);
   EXPECT_EQ(p[2], 0b01111111);
   EXPECT_EQ(p[3], 0b01010101);
   EXPECT_EQ(p[4], 0b00000000);
   EXPECT_EQ(p[5], 0b01001101);
   EXPECT_EQ(p[6], 0b01001000);
   EXPECT_EQ(p[7], 0b01011111);
   EXPECT_EQ(p[8], 0b01001000);

   p = complete_census.ptr<uchar>(4,0);
   EXPECT_EQ(p[0], 0b00000000);
   EXPECT_EQ(p[1], 0b11111101);
   EXPECT_EQ(p[2], 0b00000000);
   EXPECT_EQ(p[3], 0b10110101);
   EXPECT_EQ(p[4], 0b00000000);
   EXPECT_EQ(p[5], 0b10101101);
   EXPECT_EQ(p[6], 0b00000000);
   EXPECT_EQ(p[7], 0b10111111);
   EXPECT_EQ(p[8], 0b00000000);

   p = complete_census.ptr<uchar>(4,1);
   EXPECT_EQ(p[0], 0b10111010);
   EXPECT_EQ(p[1], 0b00000000);
   EXPECT_EQ(p[2], 0b11111110);
   EXPECT_EQ(p[3], 0b00000000);
   EXPECT_EQ(p[4], 0b01010010);
   EXPECT_EQ(p[5], 0b01011010);
   EXPECT_EQ(p[6], 0b01011110);
   EXPECT_EQ(p[7], 0b00000000);
   EXPECT_EQ(p[8], 0b11011111);

   p = complete_census.ptr<uchar>(4,2);
   EXPECT_EQ(p[0], 0b00000000);
   EXPECT_EQ(p[1], 0b11111101);
   EXPECT_EQ(p[2], 0b10101100);
   EXPECT_EQ(p[3], 0b10000100);
   EXPECT_EQ(p[4], 0b10111101);
   EXPECT_EQ(p[5], 0b10010100);
   EXPECT_EQ(p[6], 0b00000000);
   EXPECT_EQ(p[7], 0b10111111);
   EXPECT_EQ(p[8], 0b10010110);

   p = complete_census.ptr<uchar>(4,3);
   EXPECT_EQ(p[0], 0b11111011);
   EXPECT_EQ(p[1], 0b01010001);
   EXPECT_EQ(p[2], 0b00000000);
   EXPECT_EQ(p[3], 0b01110011);
   EXPECT_EQ(p[4], 0b00100001);
   EXPECT_EQ(p[5], 0b01111011);
   EXPECT_EQ(p[6], 0b01111111);
   EXPECT_EQ(p[7], 0b00101001);
   EXPECT_EQ(p[8], 0b00000000);

   p = complete_census.ptr<uchar>(4,4);
   EXPECT_EQ(p[0], 0b10101010);
   EXPECT_EQ(p[1], 0b00000000);
   EXPECT_EQ(p[2], 0b01101010);
   EXPECT_EQ(p[3], 0b01000010);
   EXPECT_EQ(p[4], 0b11111111);
   EXPECT_EQ(p[5], 0b01000010);
   EXPECT_EQ(p[6], 0b01010110);
   EXPECT_EQ(p[7], 0b00000000);
   EXPECT_EQ(p[8], 0b01010101);

   // window size 5x5
   complete_census_transform(m, complete_census, 5, true);
   CV_Assert(complete_census.channels() == 75);
   CV_Assert(complete_census.depth() == CV_8U);

   p = complete_census.ptr<uchar>(2,2);
   EXPECT_EQ(p[0], 0b00000000);
   EXPECT_EQ(p[1], 0b01000000);
   EXPECT_EQ(p[2], 0b00100000);

   EXPECT_EQ(p[3], 0b11111111);
   EXPECT_EQ(p[4], 0b11100101);
   EXPECT_EQ(p[5], 0b01111110);

   EXPECT_EQ(p[6], 0b10000000);
   EXPECT_EQ(p[7], 0b11000001);
   EXPECT_EQ(p[8], 0b00111000);

   EXPECT_EQ(p[30], 0b00000000);
   EXPECT_EQ(p[31], 0b00000000);
   EXPECT_EQ(p[32], 0b00000000);

   EXPECT_EQ(p[51], 0b11111111);
   EXPECT_EQ(p[52], 0b11111111);
   EXPECT_EQ(p[53], 0b11111111);

   EXPECT_EQ(p[57], 0b00000000);
   EXPECT_EQ(p[58], 0b00000000);
   EXPECT_EQ(p[59], 0b00000000);

   p = complete_census.ptr<uchar>(4,4);
   EXPECT_EQ(p[0], 0b01000111);
   EXPECT_EQ(p[1], 0b01101101);
   EXPECT_EQ(p[2], 0b11000100);

   EXPECT_EQ(p[3], 0b11010111);
   EXPECT_EQ(p[4], 0b01111101);
   EXPECT_EQ(p[5], 0b11010101);

   EXPECT_EQ(p[6], 0b00000010);
   EXPECT_EQ(p[7], 0b00000000);
   EXPECT_EQ(p[8], 0b10000000);
}

TEST(test_CompleteCensusTransform, test_complete_census_transform_image_8u)
{
   cv::String filename = cv::String(KFJ_DATA_PATH) + "/teddy/frame10.png";
   cv::Mat img = cv::imread(filename, cv::IMREAD_GRAYSCALE);
   if (img.empty())
   {
      std::cout << "Cannot read file: '" << filename << "'." << std::endl;
      return;
   }
   cv::Mat complete_census;
   cv::TickMeter tm;
   tm.start();
   complete_census_transform(img, complete_census, 5, true);
   tm.stop();
   std::cout << "complete_census_transform took " << tm.getTimeSec() << " s" << std::endl;
}
