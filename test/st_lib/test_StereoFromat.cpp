#include <gtest/gtest.h>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>

#include "StereoFormat.hpp"
#include "MyTools.hpp"

#include "common.hpp"

class StereoFormatTest : public ::testing::Test
{
public:
   virtual void SetUp()
   {
      m_data1.create(2, 3, CV_32FC1);
      m_data3.create(2, 3, CV_32FC3);

      //m_data1 = (((cv::Mat_<float>)m_data1) << 1, 2, 3, 4, 5, 6);
      cv::randu(m_data1, 0.0f, 120.0f);
      cv::randu(m_data3, 0.0f, 120.0f);

   }

   cv::Ptr<StereoFormat> m_format;
   cv::Mat m_data1; // gray scale image data
   cv::Mat m_data3; // color image data
};

TEST_F(StereoFormatTest, test_StereoFormat_PFM)
{
   cv::String filename = "a.pfm";

   m_format = StereoFormat::create(StereoFormat::Format::E_PFM);
   EXPECT_EQ(m_format != nullptr, true);

   //========================================
   //  Little endian 1-channel
   //========================================
   float scale = -1; //little endian
   m_format->write_disparity_to_file(filename, m_data1, &scale, 1);

   float expected_scale = 0;
   cv::Mat expected_data1;
   expected_data1 = m_format->read_disparity_from_file(filename, cv::noArray(), &expected_scale, 1);

   EXPECT_NEAR(scale, expected_scale, 1e-5);
   EXPECT_NEAR(cv::norm(m_data1-expected_data1), 0, 1e-5);

   //========================================
   //  Little endian 3-channel
   //========================================
   scale = -2; //little endian
   m_format->write_disparity_to_file(filename, m_data3, &scale, 1);

   expected_scale = 0;
   cv::Mat expected_data3;
   expected_data3 = m_format->read_disparity_from_file(filename, cv::noArray(), &expected_scale, 1);

   EXPECT_NEAR(scale, expected_scale, 1e-5);
   EXPECT_NEAR(cv::norm(m_data3-expected_data3), 0, 1e-5);

   //========================================
   //  Big endian 1-channel
   //========================================
   scale = 1; //big endian
   m_format->write_disparity_to_file(filename, m_data1, &scale, 1);

   expected_scale = 0;
   expected_data1 = m_format->read_disparity_from_file(filename, cv::noArray(), &expected_scale, 1);

   EXPECT_NEAR(scale, expected_scale, 1e-5);
   EXPECT_NEAR(cv::norm(m_data1-expected_data1), 0, 1e-5);

   //========================================
   //  Big endian 3-channel
   //========================================
   scale = 2; //big endian
   m_format->write_disparity_to_file(filename, m_data3, &scale, 1);

   expected_scale = 0;
   expected_data3 = m_format->read_disparity_from_file(filename, cv::noArray(), &expected_scale, 1);

   EXPECT_NEAR(scale, expected_scale, 1e-5);
   EXPECT_NEAR(cv::norm(m_data3-expected_data3), 0, 1e-5);

   //========================================
   //  Little endian 1-channel, valid
   //========================================
   scale = -1; //little endian
   m_data1.at<float>(0,0) = std::numeric_limits<float>::infinity();
   m_data1.at<float>(1,2) = std::numeric_limits<float>::infinity();
   m_format->write_disparity_to_file(filename, m_data1, &scale, 1);

   expected_scale = 0;
   cv::Mat valid;
   expected_data1 = m_format->read_disparity_from_file(filename, valid, &expected_scale, 1);

   EXPECT_NEAR(scale, expected_scale, 1e-5);
   EXPECT_EQ(valid.at<uchar>(0,0), 0);
   EXPECT_EQ(valid.at<uchar>(1,2), 0);
   EXPECT_EQ(cv::countNonZero(valid), 4);

   //========================================
   //  format
   //========================================
   m_format = StereoFormat::create("pfm");
   CV_Assert(m_format);

   m_format = StereoFormat::create("PFM");
   CV_Assert(m_format);

   m_format = StereoFormat::create(filename);
   CV_Assert(m_format);

   m_format = StereoFormat::create("unknown");
   CV_Assert(m_format.empty());
}

TEST_F(StereoFormatTest, test_StereoFormat_PFM_read_file)
{
   // http://vision.middlebury.edu/stereo/data/scenes2014/datasets/Sword2-perfect/disp1.pfm
   //cv::String filename = "/tmp/kuangfn/disp1.pfm";

   // http://www.pauldebevec.com/Research/HDR/memorial.pfm
   cv::String filename = cv::String(KFJ_DATA_PATH) + "/pfm/memorial.pfm";

   if (!MyTools::file_exists(filename))
   {
      printf("StereoFormatTest:PFM_read_file: File '%s' does not exist!\n", filename.c_str());
      return;
   }

   cv::Mat img;
   m_format = StereoFormat::create(filename);
   img = m_format->read_disparity_from_file(filename);

   double min_val, max_val;
   cv::minMaxIdx(img, &min_val, &max_val);
   printf("Max disparity: %.3f\nMin disparity: %.3f\n", max_val, min_val);
#if 0
   cv::normalize(img, img, 1, 0, cv::NORM_MINMAX);

   cv::namedWindow("pfm image", CV_WINDOW_NORMAL); // the image is to large, set this flag to resize the window
   cv::imshow("pfm image", img);
   cv::waitKey(0);
#endif
}
