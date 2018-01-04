#include <iostream>
#include <gtest/gtest.h>
#include <opencv2/core.hpp>

#include "MatchCost.hpp"

class MatchCostTest : public ::testing::Test
{
public:
   virtual void SetUp()
   {
      m_half_patch_size = 3;
   }
   void fill_single_channel()
   {
      m_image1.create(10, 10, CV_32FC1);
      m_image2.create(10, 10, CV_32FC1);

      cv::randu(m_image1, 0, 256);
      cv::randu(m_image2, 0, 256);
   }

   void fill_three_channel()
   {
      m_image1.create(10, 10, CV_32FC3);
      m_image2.create(10, 10, CV_32FC3);

      cv::randu(m_image1, cv::Scalar(0,0,0), cv::Scalar(256,256,256));
      cv::randu(m_image2, cv::Scalar(0,0,0), cv::Scalar(256,256,256));
   }

   cv::Ptr<MatchCost> m_cost;
   cv::Mat m_image1;
   cv::Mat m_image2;
   int m_half_patch_size;
};

TEST_F(MatchCostTest, test_SsdCost)
{
   //==================================================
   // single channel
   //--------------------------------------------------
   fill_single_channel();

   m_half_patch_size = 2;

   // outside the matrix
   cv::Point p1(3,5);
   cv::Point p2(10,2);

   m_cost = MatchCost::create("ssd");

   ASSERT_ANY_THROW(m_cost->compute_cost(m_image1, m_image2, p1, p2, m_half_patch_size));
   ASSERT_ANY_THROW(m_cost->compute_cost(m_image2, m_image1, p2, p1, m_half_patch_size));

   // inside the matrix
   p1 = cv::Point(5,6);
   p2 = cv::Point(3,5);
   double res = 0;
   for (int dy = -m_half_patch_size; dy <= m_half_patch_size; dy++)
   {
      for (int dx = -m_half_patch_size; dx <= m_half_patch_size; dx++)
      {
         float a = m_image1.at<float>(p1 + cv::Point(dx,dy));
         float b = m_image2.at<float>(p2 + cv::Point(dx,dy));
         res += (a - b) * (a - b);
      }
   }
   res = cv::sqrt(res);
   res /= (2*m_half_patch_size + 1)*(2*m_half_patch_size+1);

   double expected = m_cost->compute_cost(m_image1, m_image2, p1, p2, m_half_patch_size);
   EXPECT_NEAR(res, expected, 1e-5);

   // test a single point (patch size is 0)
   float a1 = m_image1.at<float>(p1);
   float b1 = m_image2.at<float>(p2);
   res =  cv::abs(a1-b1);

   expected = m_cost->compute_cost(m_image1, m_image2, p1, p2, 0);
   EXPECT_NEAR(res, expected, 1e-5);

   // test at boundaries
   m_half_patch_size = 2;
   p1 = cv::Point(5,0);
   p2 = cv::Point(3,5);
   cv::Mat m1 = m_image1(cv::Range(0,3), cv::Range(3, 8));
   cv::Mat m2 = m_image2(cv::Range(3, 6), cv::Range(1, 6));
   res = cv::norm(m1, m2, cv::NORM_L2) / (5*3);
   expected = m_cost->compute_cost(m_image1, m_image2, p1, p2, m_half_patch_size);
   EXPECT_NEAR(res, expected, 1e-5);

   // test at corners
   m_half_patch_size = 2;
   p1 = cv::Point(9,9);
   p2 = cv::Point(0,5);
   m1 = m_image1(cv::Range(7, 10), cv::Range(7,10));
   m2 = m_image2(cv::Range(3, 6), cv::Range(0, 3));
   res = cv::norm(m1, m2, cv::NORM_L2) / (3 * 3);
   expected = m_cost->compute_cost(m_image1, m_image2, p1, p2, m_half_patch_size);
   EXPECT_NEAR(res, expected, 1e-5);

   // test 3 channels
   fill_three_channel();
   m_half_patch_size = 2;
   p1 = cv::Point(3, 5);
   p2 = cv::Point(5, 3);
   res = 0;
   for (int dy = -m_half_patch_size; dy <= m_half_patch_size; dy++)
   {
      for (int dx = -m_half_patch_size; dx <= m_half_patch_size; dx++)
      {
         cv::Vec3f a = m_image1.at<cv::Vec3f>(p1 + cv::Point(dx,dy));
         cv::Vec3f b = m_image2.at<cv::Vec3f>(p2 + cv::Point(dx,dy));
         //res +=  cv::norm(a,b,cv::NORM_L2SQR);
         res += (a[0]-b[0])*(a[0]-b[0]) + (a[1]-b[1])*(a[1]-b[1]) + (a[2]-b[2])*(a[2]-b[2]);
      }
   }
   res = cv::sqrt(res);
   res /= (2*m_half_patch_size + 1)*(2*m_half_patch_size+1)*3;

   expected = m_cost->compute_cost(m_image1, m_image2, p1, p2, m_half_patch_size);
   EXPECT_NEAR(res, expected, 1e-5);

   // test a single point (patch size is 0)
   cv::Vec3f a = m_image1.at<cv::Vec3f>(p1);
   cv::Vec3f b = m_image2.at<cv::Vec3f>(p2);
   res =  cv::norm(a,b,cv::NORM_L2);
   res /= 3;

   expected = m_cost->compute_cost(m_image1, m_image2, p1, p2, 0);
   EXPECT_NEAR(res, expected, 1e-5);

   // test at boundaries
   m_half_patch_size = 2;
   p1 = cv::Point(5,0);
   p2 = cv::Point(3,5);
   m1 = m_image1(cv::Range(0,3), cv::Range(3, 8));
   m2 = m_image2(cv::Range(3, 6), cv::Range(1, 6));
   res = cv::norm(m1, m2, cv::NORM_L2) / (5*3*3);
   expected = m_cost->compute_cost(m_image1, m_image2, p1, p2, m_half_patch_size);
   EXPECT_NEAR(res, expected, 1e-5);

   // test at corners
   m_half_patch_size = 2;
   p1 = cv::Point(9,9);
   p2 = cv::Point(0,5);
   m1 = m_image1(cv::Range(7, 10), cv::Range(7,10));
   m2 = m_image2(cv::Range(3, 6), cv::Range(0, 3));
   res = cv::norm(m1, m2, cv::NORM_L2) / (3 * 3 * 3);
   expected = m_cost->compute_cost(m_image1, m_image2, p1, p2, m_half_patch_size);
   EXPECT_NEAR(res, expected, 1e-5);
}

TEST_F(MatchCostTest, test_SadCost)
{
   //==================================================
   // single channel
   //--------------------------------------------------
   fill_single_channel();

   m_half_patch_size = 2;

   // outside the matrix
   cv::Point p1(3,5);
   cv::Point p2(10,2);

   m_cost = MatchCost::create("sad");

   ASSERT_ANY_THROW(m_cost->compute_cost(m_image1, m_image2, p1, p2, m_half_patch_size));
   ASSERT_ANY_THROW(m_cost->compute_cost(m_image2, m_image1, p2, p1, m_half_patch_size));

   // inside the matrix
   p1 = cv::Point(5,6);
   p2 = cv::Point(3,5);
   double res = 0;
   for (int dy = -m_half_patch_size; dy <= m_half_patch_size; dy++)
   {
      for (int dx = -m_half_patch_size; dx <= m_half_patch_size; dx++)
      {
         float a = m_image1.at<float>(p1 + cv::Point(dx,dy));
         float b = m_image2.at<float>(p2 + cv::Point(dx,dy));
         res += cv::abs(a - b);
      }
   }
   res /= (2*m_half_patch_size + 1)*(2*m_half_patch_size+1);

   double expected = m_cost->compute_cost(m_image1, m_image2, p1, p2, m_half_patch_size);
   EXPECT_NEAR(res, expected, 1e-5);

   // test a single point (patch size is 0)
   float a1 = m_image1.at<float>(p1);
   float b1 = m_image2.at<float>(p2);
   res =  cv::abs(a1-b1);

   expected = m_cost->compute_cost(m_image1, m_image2, p1, p2, 0);
   EXPECT_NEAR(res, expected, 1e-5);

   // test at boundaries
   m_half_patch_size = 2;
   p1 = cv::Point(5,0);
   p2 = cv::Point(3,5);
   cv::Mat m1 = m_image1(cv::Range(0,3), cv::Range(3, 8));
   cv::Mat m2 = m_image2(cv::Range(3, 6), cv::Range(1, 6));
   res = cv::norm(m1, m2, cv::NORM_L1) / (5*3);
   expected = m_cost->compute_cost(m_image1, m_image2, p1, p2, m_half_patch_size);
   EXPECT_NEAR(res, expected, 1e-5);

   // test at corners
   m_half_patch_size = 2;
   p1 = cv::Point(9,9);
   p2 = cv::Point(0,5);
   m1 = m_image1(cv::Range(7, 10), cv::Range(7,10));
   m2 = m_image2(cv::Range(3, 6), cv::Range(0, 3));
   res = cv::norm(m1, m2, cv::NORM_L1) / (3 * 3);
   expected = m_cost->compute_cost(m_image1, m_image2, p1, p2, m_half_patch_size);
   EXPECT_NEAR(res, expected, 1e-5);

   // test 3 channels
   fill_three_channel();
   m_half_patch_size = 2;
   p1 = cv::Point(3, 5);
   p2 = cv::Point(5, 3);
   res = 0;
   for (int dy = -m_half_patch_size; dy <= m_half_patch_size; dy++)
   {
      for (int dx = -m_half_patch_size; dx <= m_half_patch_size; dx++)
      {
         cv::Vec3f a = m_image1.at<cv::Vec3f>(p1 + cv::Point(dx,dy));
         cv::Vec3f b = m_image2.at<cv::Vec3f>(p2 + cv::Point(dx,dy));
         //res +=  cv::norm(a,b,cv::NORM_L1);
         res += cv::abs(a[0]-b[0]) + cv::abs(a[1]-b[1]) + cv::abs(a[2]-b[2]);
      }
   }
   res /= (2*m_half_patch_size + 1)*(2*m_half_patch_size+1)*3;

   expected = m_cost->compute_cost(m_image1, m_image2, p1, p2, m_half_patch_size);
   EXPECT_NEAR(res, expected, 1e-5);

   // test a single point (patch size is 0)
   cv::Vec3f a = m_image1.at<cv::Vec3f>(p1);
   cv::Vec3f b = m_image2.at<cv::Vec3f>(p2);
   res =  cv::norm(a,b,cv::NORM_L1);
   res /= 3;

   expected = m_cost->compute_cost(m_image1, m_image2, p1, p2, 0);
   EXPECT_NEAR(res, expected, 1e-5);

   // test at boundaries
   m_half_patch_size = 2;
   p1 = cv::Point(5,0);
   p2 = cv::Point(3,5);
   m1 = m_image1(cv::Range(0,3), cv::Range(3, 8));
   m2 = m_image2(cv::Range(3, 6), cv::Range(1, 6));
   res = cv::norm(m1, m2, cv::NORM_L1) / (5*3*3);
   expected = m_cost->compute_cost(m_image1, m_image2, p1, p2, m_half_patch_size);
   EXPECT_NEAR(res, expected, 1e-5);

   // test at corners
   m_half_patch_size = 2;
   p1 = cv::Point(9,9);
   p2 = cv::Point(0,5);
   m1 = m_image1(cv::Range(7, 10), cv::Range(7,10));
   m2 = m_image2(cv::Range(3, 6), cv::Range(0, 3));
   res = cv::norm(m1, m2, cv::NORM_L1) / (3 * 3 * 3);
   expected = m_cost->compute_cost(m_image1, m_image2, p1, p2, m_half_patch_size);
   EXPECT_NEAR(res, expected, 1e-5);
}

TEST_F(MatchCostTest, test_HammingCost)
{
   // single channel
   m_cost = MatchCost::create("hamming");
   uchar a[] = {0b11000011, 0b00001000, 0b11110000, 0b00001111, 0b00001111, 0b00001100, 0b00000000, 0b00000001,
                0b00110000, 0b11110011, 0b00000001, 0b01111110, 0b00001011, 0b10101000, 0b00000000, 0b00000001,
                0b00100000, 0b10000000, 0b00001000, 0b00000001, 0b00001100, 0b01100100, 0b00000001, 0b00000001,
                0b01010000, 0b10100000, 0b00010001, 0b01000001, 0b00001010, 0b00001010, 0b00000001, 0b00000001,
                0b01011001, 0b00100001, 0b10110101, 0b01001111, 0b10100110, 0b00110110, 0b00000001, 0b00000001,
   };

   // changed (1,1), (2,3)

   uchar b[] = {0b11000011, 0b00001000, 0b11110000, 0b00001111, 0b00001111, 0b00001100, 0b00000001, 0b00000001,
                0b00110000, 0b11111111, 0b00000001, 0b01111110, 0b00001011, 0b10101000, 0b00000001, 0b00000001,
                0b00100000, 0b10000000, 0b00001000, 0b00110001, 0b00001100, 0b01100100, 0b00000001, 0b00000001,
                0b01010000, 0b10100000, 0b00010001, 0b01000001, 0b00001010, 0b00001010, 0b00000001, 0b00000001,
                0b01011001, 0b00100001, 0b10110101, 0b01001111, 0b10100110, 0b00110110, 0b00000001, 0b00000001,
   };

   cv::Mat ma(5, 8, CV_8UC1, a);
   cv::Mat mb(5, 8, CV_8UC1, b);
   double expected = 2/9.;
   double res = m_cost->compute_cost(ma, mb, cv::Point(1,1), cv::Point(1,1), 1);
   EXPECT_NEAR(expected, res, 1e-5);

   expected = 2.;
   res = m_cost->compute_cost(ma, mb, cv::Point(1,1), cv::Point(1,1), 0);
   EXPECT_NEAR(expected, res, 1e-5);

   expected = 4./9;
   res = m_cost->compute_cost(ma, mb, cv::Point(2,2), cv::Point(2,2), 1);
   EXPECT_NEAR(expected, res, 1e-5);

   // two channels
   cv::Mat mc(5, 4, CV_8UC2, a);
   cv::Mat md(5, 4, CV_8UC2, b);

   std::cout << "two channels ...." << std::endl;
   expected = 2/2.;
   res = m_cost->compute_cost(mc, md, cv::Point(1,2), cv::Point(1,2), 0);
   EXPECT_NEAR(expected, res, 1e-5);

   expected = 0;
   res = m_cost->compute_cost(mc, md, cv::Point(1,1), cv::Point(1,1), 0);
   EXPECT_NEAR(expected, res, 1e-5);

   expected = 4/18.;
   res = m_cost->compute_cost(mc, md, cv::Point(1,1), cv::Point(1,1), 1);
   EXPECT_NEAR(expected, res, 1e-5);

   expected = 2/18.;
   res = m_cost->compute_cost(mc, md, cv::Point(2,3), cv::Point(2,3), 1);
   EXPECT_NEAR(expected, res, 1e-5);
}
