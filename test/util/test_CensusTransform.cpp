#include <iostream>
#include <gtest/gtest.h>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgcodecs.hpp>

#include "CensusTransform.hpp"
#include "common.hpp"

TEST(test_CensusTransform, test_census_transform_8u)
{
   uchar d[] = {
        1,  20,  3,  5,  6,
        10, 9,  11,  3,  1,
        0,  5,  22,  30, 4,
        20, 1,  33,  9,  0,
        1,  2,  10,  8,  23
   };

   cv::Mat m(5, 5, CV_8UC1, d);
   cv::Mat census;

   // window size 3x3
   census_transform(m, census, 3);

   uchar a00 = census.at<uchar>(0,0);
   uchar a01 = census.at<uchar>(0,1);
   uchar a02 = census.at<uchar>(0,2);
   uchar a03 = census.at<uchar>(0,3);
   uchar a04 = census.at<uchar>(0,4);

   EXPECT_EQ(a00, 0b00000000);
   EXPECT_EQ(a01, 0b11111111);
   EXPECT_EQ(a02, 0b00000000);
   EXPECT_EQ(a03, 0b01110011);
   EXPECT_EQ(a04, 0b11111111);

   uchar a10 = census.at<uchar>(1,0);
   uchar a11 = census.at<uchar>(1,1);
   uchar a12 = census.at<uchar>(1,2);
   uchar a13 = census.at<uchar>(1,3);
   uchar a14 = census.at<uchar>(1,4);
   EXPECT_EQ(a10, 0b01011111);
   EXPECT_EQ(a11, 0b10100110);
   EXPECT_EQ(a12, 0b01111100);
   EXPECT_EQ(a13, 0b00001000);
   EXPECT_EQ(a14, 0b00000000);

   uchar a20 = census.at<uchar>(2,0);
   uchar a21 = census.at<uchar>(2,1);
   uchar a22 = census.at<uchar>(2,2);
   uchar a23 = census.at<uchar>(2,3);
   uchar a24 = census.at<uchar>(2,4);
   EXPECT_EQ(a20, 0b00000000);
   EXPECT_EQ(a21, 0b00010010);
   EXPECT_EQ(a22, 0b11110101);
   EXPECT_EQ(a23, 0b11111011);
   EXPECT_EQ(a24, 0b11100010);

   uchar a30 = census.at<uchar>(3,0);
   uchar a31 = census.at<uchar>(3,1);
   uchar a32 = census.at<uchar>(3,2);
   uchar a33 = census.at<uchar>(3,3);
   uchar a34 = census.at<uchar>(3,4);
   EXPECT_EQ(a30, 0b11111111);
   EXPECT_EQ(a31, 0b10000000);
   EXPECT_EQ(a32, 0b11111111);
   EXPECT_EQ(a33, 0b00101010);
   EXPECT_EQ(a34, 0b00000000);

   uchar a40 = census.at<uchar>(4,0);
   uchar a41 = census.at<uchar>(4,1);
   uchar a42 = census.at<uchar>(4,2);
   uchar a43 = census.at<uchar>(4,3);
   uchar a44 = census.at<uchar>(4,4);
   EXPECT_EQ(a40, 0b00000000);
   EXPECT_EQ(a41, 0b01010010);
   EXPECT_EQ(a42, 0b10111101);
   EXPECT_EQ(a43, 0b00100001);
   EXPECT_EQ(a44, 0b11111111);

   // window size 5x5
   census_transform(m, census, 5);

   uchar a000 = census.at<cv::Vec3b>(0,0)[0];
   uchar a001 = census.at<cv::Vec3b>(0,0)[1];
   uchar a002 = census.at<cv::Vec3b>(0,0)[2];
   EXPECT_EQ(a000, 0b00100000);
   EXPECT_EQ(a001, 0b00000000);
   EXPECT_EQ(a002, 0b00000100);

   uchar a010 = census.at<cv::Vec3b>(0,1)[0];
   uchar a011 = census.at<cv::Vec3b>(0,1)[1];
   uchar a012 = census.at<cv::Vec3b>(0,1)[2];
   EXPECT_EQ(a010, 0b11100111);
   EXPECT_EQ(a011, 0b11011111);
   EXPECT_EQ(a012, 0b11111100);

   uchar a020 = census.at<cv::Vec3b>(0,2)[0];
   uchar a021 = census.at<cv::Vec3b>(0,2)[1];
   uchar a022 = census.at<cv::Vec3b>(0,2)[2];
   EXPECT_EQ(a020, 0b10000000);
   EXPECT_EQ(a021, 0b01100000);
   EXPECT_EQ(a022, 0b00110000);

   uchar a030 = census.at<cv::Vec3b>(0,3)[0];
   uchar a031 = census.at<cv::Vec3b>(0,3)[1];
   uchar a032 = census.at<cv::Vec3b>(0,3)[2];
   EXPECT_EQ(a030, 0b00010001);
   EXPECT_EQ(a031, 0b11010000);
   EXPECT_EQ(a032, 0b11100010);

   uchar a040 = census.at<cv::Vec3b>(0,4)[0];
   uchar a041 = census.at<cv::Vec3b>(0,4)[1];
   uchar a042 = census.at<cv::Vec3b>(0,4)[2];
   EXPECT_EQ(a040, 0b00100011);
   EXPECT_EQ(a041, 0b10111101);
   EXPECT_EQ(a042, 0b11000100);

   uchar a100 = census.at<cv::Vec3b>(1,0)[0];
   uchar a101 = census.at<cv::Vec3b>(1,0)[1];
   uchar a102 = census.at<cv::Vec3b>(1,0)[2];
   EXPECT_EQ(a100, 0b01010101);
   EXPECT_EQ(a101, 0b01011001);
   EXPECT_EQ(a102, 0b11001010);

   uchar a110 = census.at<cv::Vec3b>(1,1)[0];
   uchar a111 = census.at<cv::Vec3b>(1,1)[1];
   uchar a112 = census.at<cv::Vec3b>(1,1)[2];
   EXPECT_EQ(a110, 0b00001010);
   EXPECT_EQ(a111, 0b11000111);
   EXPECT_EQ(a112, 0b10010100);

   uchar a120 = census.at<cv::Vec3b>(1,2)[0];
   uchar a121 = census.at<cv::Vec3b>(1,2)[1];
   uchar a122 = census.at<cv::Vec3b>(1,2)[2];
   EXPECT_EQ(a120, 0b11011101);
   EXPECT_EQ(a121, 0b11111111);
   EXPECT_EQ(a122, 0b00101011);

   uchar a130 = census.at<cv::Vec3b>(1,3)[0];
   uchar a131 = census.at<cv::Vec3b>(1,3)[1];
   uchar a132 = census.at<cv::Vec3b>(1,3)[2];
   EXPECT_EQ(a130, 0b00010000);
   EXPECT_EQ(a131, 0b00001000);
   EXPECT_EQ(a132, 0b00010010);

   uchar a140 = census.at<cv::Vec3b>(1,4)[0];
   uchar a141 = census.at<cv::Vec3b>(1,4)[1];
   uchar a142 = census.at<cv::Vec3b>(1,4)[2];
   EXPECT_EQ(a140, 0b00000000);
   EXPECT_EQ(a141, 0b00000000);
   EXPECT_EQ(a142, 0b00000100);

   uchar a200 = census.at<cv::Vec3b>(2,0)[0];
   uchar a201 = census.at<cv::Vec3b>(2,0)[1];
   uchar a202 = census.at<cv::Vec3b>(2,0)[2];
   EXPECT_EQ(a200, 0b00000000);
   EXPECT_EQ(a201, 0b00000000);
   EXPECT_EQ(a202, 0b00000000);

   uchar a210 = census.at<cv::Vec3b>(2,1)[0];
   uchar a211 = census.at<cv::Vec3b>(2,1)[1];
   uchar a212 = census.at<cv::Vec3b>(2,1)[2];
   EXPECT_EQ(a210, 0b01010000);
   EXPECT_EQ(a211, 0b01010010);
   EXPECT_EQ(a212, 0b10011100);

   uchar a220 = census.at<cv::Vec3b>(2,2)[0];
   uchar a221 = census.at<cv::Vec3b>(2,2)[1];
   uchar a222 = census.at<cv::Vec3b>(2,2)[2];
   EXPECT_EQ(a220, 0b11111111);
   EXPECT_EQ(a221, 0b11110111);
   EXPECT_EQ(a222, 0b01111110);

   uchar a230 = census.at<cv::Vec3b>(2,3)[0];
   uchar a231 = census.at<cv::Vec3b>(2,3)[1];
   uchar a232 = census.at<cv::Vec3b>(2,3)[2];
   EXPECT_EQ(a230, 0b11111111);
   EXPECT_EQ(a231, 0b11111010);
   EXPECT_EQ(a232, 0b11111111);

   uchar a240 = census.at<cv::Vec3b>(2,4)[0];
   uchar a241 = census.at<cv::Vec3b>(2,4)[1];
   uchar a242 = census.at<cv::Vec3b>(2,4)[2];
   EXPECT_EQ(a240, 0b10001011);
   EXPECT_EQ(a241, 0b10000000);
   EXPECT_EQ(a242, 0b10000000);

   uchar a300 = census.at<cv::Vec3b>(3,0)[0];
   uchar a301 = census.at<cv::Vec3b>(3,0)[1];
   uchar a302 = census.at<cv::Vec3b>(3,0)[2];
   EXPECT_EQ(a300, 0b11111011);
   EXPECT_EQ(a301, 0b10011011);
   EXPECT_EQ(a302, 0b11101010);

   uchar a310 = census.at<cv::Vec3b>(3,1)[0];
   uchar a311 = census.at<cv::Vec3b>(3,1)[1];
   uchar a312 = census.at<cv::Vec3b>(3,1)[2];
   EXPECT_EQ(a310, 0b00000010);
   EXPECT_EQ(a311, 0b00000000);
   EXPECT_EQ(a312, 0b00000000);

   uchar a320 = census.at<cv::Vec3b>(3,2)[0];
   uchar a321 = census.at<cv::Vec3b>(3,2)[1];
   uchar a322 = census.at<cv::Vec3b>(3,2)[2];
   EXPECT_EQ(a320, 0b11111111);
   EXPECT_EQ(a321, 0b11111111);
   EXPECT_EQ(a322, 0b11111011);

   uchar a330 = census.at<cv::Vec3b>(3,3)[0];
   uchar a331 = census.at<cv::Vec3b>(3,3)[1];
   uchar a332 = census.at<cv::Vec3b>(3,3)[2];
   EXPECT_EQ(a330, 0b00111100);
   EXPECT_EQ(a331, 0b10101010);
   EXPECT_EQ(a332, 0b10110010);

   uchar a340 = census.at<cv::Vec3b>(3,4)[0];
   uchar a341 = census.at<cv::Vec3b>(3,4)[1];
   uchar a342 = census.at<cv::Vec3b>(3,4)[2];
   EXPECT_EQ(a340, 0b00000000);
   EXPECT_EQ(a341, 0b00000000);
   EXPECT_EQ(a342, 0b00000000);

   uchar a400 = census.at<cv::Vec3b>(4,0)[0];
   uchar a401 = census.at<cv::Vec3b>(4,0)[1];
   uchar a402 = census.at<cv::Vec3b>(4,0)[2];
   EXPECT_EQ(a400, 0b00100000);
   EXPECT_EQ(a401, 0b00000000);
   EXPECT_EQ(a402, 0b00000100);

   uchar a410 = census.at<cv::Vec3b>(4,1)[0];
   uchar a411 = census.at<cv::Vec3b>(4,1)[1];
   uchar a412 = census.at<cv::Vec3b>(4,1)[2];
   EXPECT_EQ(a410, 0b01000101);
   EXPECT_EQ(a411, 0b00010010);
   EXPECT_EQ(a412, 0b10001000);

   uchar a420 = census.at<cv::Vec3b>(4,2)[0];
   uchar a421 = census.at<cv::Vec3b>(4,2)[1];
   uchar a422 = census.at<cv::Vec3b>(4,2)[2];
   EXPECT_EQ(a420, 0b11001010);
   EXPECT_EQ(a421, 0b11111001);
   EXPECT_EQ(a422, 0b01111001);

   uchar a430 = census.at<cv::Vec3b>(4,3)[0];
   uchar a431 = census.at<cv::Vec3b>(4,3)[1];
   uchar a432 = census.at<cv::Vec3b>(4,3)[2];
   EXPECT_EQ(a430, 0b10010100);
   EXPECT_EQ(a431, 0b10100010);
   EXPECT_EQ(a432, 0b01010010);

   uchar a440 = census.at<cv::Vec3b>(4,4)[0];
   uchar a441 = census.at<cv::Vec3b>(4,4)[1];
   uchar a442 = census.at<cv::Vec3b>(4,4)[2];
   EXPECT_EQ(a440, 0b10101011);
   EXPECT_EQ(a441, 0b10111101);
   EXPECT_EQ(a442, 0b11010101);
}

TEST(test_CensusTransform, test_census_transform_image_8u)
{
   cv::String filename = cv::String(KFJ_DATA_PATH) + "/teddy/frame10.png";

   cv::Mat img = cv::imread(filename, cv::IMREAD_GRAYSCALE);
   if (img.empty())
   {
      std::cout << "Cannot read file: '" << filename << "'." << std::endl;
      return;
   }

   cv::Mat census;
   cv::TickMeter tm;
   tm.start();
   census_transform(img, census, 5);
   tm.stop();
   std::cout << "census_transform took " << tm.getTimeSec() << " s" << std::endl;

   cv::imwrite("teddy-census-5x5.png", census);
#if 0
   cv::namedWindow("census", CV_WINDOW_NORMAL);
   cv::imshow("census", census);
   cv::waitKey(0);
#endif
}
