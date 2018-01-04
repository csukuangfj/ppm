#include <string>
#include <gtest/gtest.h>

#include <opencv2/highgui.hpp>

#include "MyImageProcessing.hpp"
#include "MyTools.hpp"

#include "common.hpp"

class MyImageProcessingTest : public ::testing::Test
{
public:
   virtual void SetUp()
   {
      int ny = 7;
      int nx = 6;
      int num_channels = 3;
      int n = ny*nx*num_channels;
      std::vector<float> d;
      for (int i = 0; i < n; i++)
      {
         d.push_back(i);
      }

      m_m = cv::Mat(ny, nx, CV_32FC(num_channels), d.data()).clone();

      std::vector<cv::Mat> mv;
      mv.resize(num_channels);
      cv::split(m_m, mv.data());

      m_m1 = mv[0];
      m_m2 = mv[1];
      m_m3 = mv[2];
   }
   cv::Mat m_m; //!< CV_32FC3, 6-by-6 matrix, with 3 channels
   cv::Mat m_m1;
   cv::Mat m_m2;
   cv::Mat m_m3;
};


TEST_F(MyImageProcessingTest, test_pyramid_ratio)
{
   cv::String f1_name = cv::String(KFJ_DATA_PATH) + "/teddy/im2.png";

   if (!MyTools::file_exists(f1_name))
   {
      printf("File '%s' does not exist\n", f1_name.c_str());
      return;
   }

   cv::Mat f1;
   f1 = cv::imread(f1_name, cv::IMREAD_COLOR);

   float ratio = 0.5;
   int min_width = 30;

   int num_levels;

   std::vector<cv::Mat> pyramid;

   num_levels = MyImageProcessing::get_image_pyramid_by_ratio(f1, pyramid,
                                                            ratio, min_width);

   std::cout << std::endl
             << "Raw height:" << f1.rows << std::endl
             << "Raw width:" << f1.cols << std::endl
             << "minimum width:" << min_width << std::endl
             << "ratio: " << ratio << std::endl
             << "num_levels: " << num_levels << std::endl
             << std::endl;

#if 0
   cv::Mat img;
   MyTools::concatenate_images(pyramid, img);

   cv::String s;
   s = cv::format("fixed ratio:%.2f, num_levels:%d", ratio, num_levels);
   cv::namedWindow(s, CV_WINDOW_NORMAL);
   cv::imshow(s, img);

   for (int i = 0; i < num_levels; i++)
   {
      s = cv::format("level %d", i);
      //cv::imshow(s, pyramid[i]);
      s = cv::format("level %d\nheight:%d, width:%d\n", i, pyramid[i].rows,
                                                        pyramid[i].cols);
      std::cout << s << std::endl;
   }
   cv::waitKey(0);
#endif
}

TEST_F(MyImageProcessingTest, test_pyramid_level)
{
   cv::String f1_name = cv::String(KFJ_DATA_PATH) + "/teddy/im2.png";

   if (!MyTools::file_exists(f1_name))
   {
      printf("File '%s' does not exist\n", f1_name.c_str());
      return;
   }

   cv::Mat f1;
   f1 = cv::imread(f1_name, cv::IMREAD_COLOR);
   f1.convertTo(f1, CV_32F);

   float ratio = 0.5;
   int min_width = 30;

   int expected_levels = 3;
   int num_levels = expected_levels;

   std::vector<cv::Mat> pyramid;

   num_levels = MyImageProcessing::get_image_pyramid_by_levels(f1, pyramid,
                                                             ratio, num_levels,
                                                             min_width);
   CV_Assert(num_levels == expected_levels);

   std::cout << std::endl
             << "Raw height:" << f1.rows << std::endl
             << "Raw width:" << f1.cols << std::endl
             << "minimum width:" << min_width << std::endl
             << "ratio: " << ratio << std::endl
             << "num_levels: " << num_levels << std::endl
             << std::endl;

   for (auto& pp : pyramid)
   {
      pp.convertTo(pp, CV_8U);
   }

   cv::Mat img;
   MyTools::concatenate_images(pyramid, img);

   cv::imwrite("teddy-pyramid.png", img);
#if 0

   cv::String s;
   s = cv::format("ratio:%.2f, fixed num_levels:%d", ratio, num_levels);
   cv::namedWindow(s, CV_WINDOW_NORMAL);
   cv::imshow(s, img);

   for (int i = 0; i < num_levels; i++)
   {
      s = cv::format("level %d", i);
      //cv::imshow(s, pyramid[i]);
      s = cv::format("level %d\nheight:%d, width:%d\n", i, pyramid[i].rows,
                                                        pyramid[i].cols);
      std::cout << s << std::endl;

      cv::imshow(s, pyramid[i]);
   }
   cv::waitKey(0);
#endif
}


/*
m1:
[0,  3,   6,  9, 12, 15;
 18, 21, 24, 27, 30, 33;
 36, 39, 42, 45, 48, 51;
 54, 57, 60, 63, 66, 69;
 72, 75, 78, 81, 84, 87;
 90, 93, 96, 99, 102, 105;
 108, 111, 114, 117, 120, 123]
--------
m2:
[ 1,  4,  7, 10, 13, 16;
 19, 22, 25, 28, 31, 34;
 37, 40, 43, 46, 49, 52;
 55, 58, 61, 64, 67, 70;
 73, 76, 79, 82, 85, 88;
 91, 94, 97, 100, 103, 106;
 109, 112, 115, 118, 121, 124]
--------
m3:
[2, 5, 8, 11, 14, 17;
 20, 23, 26, 29, 32, 35;
 38, 41, 44, 47, 50, 53;
 56, 59, 62, 65, 68, 71;
 74, 77, 80, 83, 86, 89;
 92, 95, 98, 101, 104, 107;
 110, 113, 116, 119, 122, 125]

octave:
 interp2(m1, x1, y1, "cubic") // nearest, linear
(note that it handles boundary differently)
 */
TEST_F(MyImageProcessingTest, test_bicubic_interpolation_1channel)
{
   float x1 = 3.2, y1 = 4.5;
   cv::Mat res;

   res = MyImageProcessing::bicubic_interpolation(m_m1, x1-1, y1-1);
   EXPECT_NEAR(res.at<float>(0,0), 69.600f, 1e-5);

   res = MyImageProcessing::bicubic_interpolation(m_m2, x1-1, y1-1);
   EXPECT_NEAR(res.at<float>(0,0), 70.600f, 1e-5);

   res = MyImageProcessing::bicubic_interpolation(m_m3, x1-1, y1-1);
   EXPECT_NEAR(res.at<float>(0,0), 71.600f, 1e-5);

   res = MyImageProcessing::bicubic_interpolation(m_m, x1-1, y1-1);
   EXPECT_NEAR(res.ptr<float>(0,0)[0], 69.600f, 1e-5);
   EXPECT_NEAR(res.ptr<float>(0,0)[1], 70.600f, 1e-5);
   EXPECT_NEAR(res.ptr<float>(0,0)[2], 71.600f, 1e-5);

   x1 = 3.6;

   res = MyImageProcessing::bicubic_interpolation(m_m1, x1-1, y1-1);
   EXPECT_NEAR(res.at<float>(0,0), 70.800f, 1e-5);

   res = MyImageProcessing::bicubic_interpolation(m_m2, x1-1, y1-1);
   EXPECT_NEAR(res.at<float>(0,0), 71.800f, 1e-5);

   res = MyImageProcessing::bicubic_interpolation(m_m3, x1-1, y1-1);
   EXPECT_NEAR(res.at<float>(0,0), 72.800f, 1e-5);

   res = MyImageProcessing::bicubic_interpolation(m_m, x1-1, y1-1);
   EXPECT_NEAR(res.ptr<float>(0,0)[0], 70.800f, 1e-5);
   EXPECT_NEAR(res.ptr<float>(0,0)[1], 71.800f, 1e-5);
   EXPECT_NEAR(res.ptr<float>(0,0)[2], 72.800f, 1e-5);
}

TEST_F(MyImageProcessingTest, test_nearest_neighbor_interpolation)
{
   float x1 = 3.2, y1 = 4.5;
   cv::Mat res;

   res = MyImageProcessing::nearest_neighbor_interpolation(m_m1, x1-1, y1-1);
   EXPECT_NEAR(res.at<float>(0,0), 78.f, 1e-5);

   res = MyImageProcessing::nearest_neighbor_interpolation(m_m2, x1-1, y1-1);
   EXPECT_NEAR(res.at<float>(0,0), 79.f, 1e-5);

   res = MyImageProcessing::nearest_neighbor_interpolation(m_m3, x1-1, y1-1);
   EXPECT_NEAR(res.at<float>(0,0), 80.f, 1e-5);

   res = MyImageProcessing::nearest_neighbor_interpolation(m_m, x1-1, y1-1);
   EXPECT_NEAR(res.ptr<float>(0,0)[0], 78.f, 1e-5);
   EXPECT_NEAR(res.ptr<float>(0,0)[1], 79.f, 1e-5);
   EXPECT_NEAR(res.ptr<float>(0,0)[2], 80.f, 1e-5);

   x1 = 3.6;

   res = MyImageProcessing::nearest_neighbor_interpolation(m_m1, x1-1, y1-1);
   EXPECT_NEAR(res.at<float>(0,0), 81.f, 1e-5);

   res = MyImageProcessing::nearest_neighbor_interpolation(m_m2, x1-1, y1-1);
   EXPECT_NEAR(res.at<float>(0,0), 82.f, 1e-5);

   res = MyImageProcessing::nearest_neighbor_interpolation(m_m3, x1-1, y1-1);
   EXPECT_NEAR(res.at<float>(0,0), 83.f, 1e-5);

   res = MyImageProcessing::nearest_neighbor_interpolation(m_m, x1-1, y1-1);
   EXPECT_NEAR(res.ptr<float>(0,0)[0], 81.f, 1e-5);
   EXPECT_NEAR(res.ptr<float>(0,0)[1], 82.f, 1e-5);
   EXPECT_NEAR(res.ptr<float>(0,0)[2], 83.f, 1e-5);

   // boundary
   x1 = -1.8f;
   y1 = 6.2f;
   res = MyImageProcessing::nearest_neighbor_interpolation(m_m1, x1, y1);
   EXPECT_NEAR(res.at<float>(0,0), 108.f, 1e-5);

   res = MyImageProcessing::nearest_neighbor_interpolation(m_m2, x1, y1);
   EXPECT_NEAR(res.at<float>(0,0), 109.f, 1e-5);

   res = MyImageProcessing::nearest_neighbor_interpolation(m_m3, x1, y1);
   EXPECT_NEAR(res.at<float>(0,0), 110.f, 1e-5);

   res = MyImageProcessing::nearest_neighbor_interpolation(m_m, x1, y1);
   EXPECT_NEAR(res.ptr<float>(0,0)[0], 108.f, 1e-5);
   EXPECT_NEAR(res.ptr<float>(0,0)[1], 109.f, 1e-5);
   EXPECT_NEAR(res.ptr<float>(0,0)[2], 110.f, 1e-5);
}

TEST_F(MyImageProcessingTest, test_bilinear_interpolation)
{
   float x1 = 3.2, y1 = 4.5;
   cv::Mat res;

   res = MyImageProcessing::bilinear_interpolation(m_m1, x1-1, y1-1);
   EXPECT_NEAR(res.at<float>(0,0), 69.6f, 1e-5);

   res = MyImageProcessing::bilinear_interpolation(m_m2, x1-1, y1-1);
   EXPECT_NEAR(res.at<float>(0,0), 70.600f, 1e-5);

   res = MyImageProcessing::bilinear_interpolation(m_m3, x1-1, y1-1);
   EXPECT_NEAR(res.at<float>(0,0), 71.600f, 1e-5);

   res = MyImageProcessing::bilinear_interpolation(m_m, x1-1, y1-1);
   EXPECT_NEAR(res.ptr<float>(0,0)[0], 69.600f, 1e-5);
   EXPECT_NEAR(res.ptr<float>(0,0)[1], 70.600f, 1e-5);
   EXPECT_NEAR(res.ptr<float>(0,0)[2], 71.600f, 1e-5);

   x1 = 3.6;

   res = MyImageProcessing::bilinear_interpolation(m_m1, x1-1, y1-1);
   EXPECT_NEAR(res.at<float>(0,0), 70.800f, 1e-5);

   res = MyImageProcessing::bilinear_interpolation(m_m2, x1-1, y1-1);
   EXPECT_NEAR(res.at<float>(0,0), 71.800f, 1e-5);

   res = MyImageProcessing::bilinear_interpolation(m_m3, x1-1, y1-1);
   EXPECT_NEAR(res.at<float>(0,0), 72.800f, 1e-5);

   res = MyImageProcessing::bilinear_interpolation(m_m, x1-1, y1-1);
   EXPECT_NEAR(res.ptr<float>(0,0)[0], 70.800f, 1e-5);
   EXPECT_NEAR(res.ptr<float>(0,0)[1], 71.800f, 1e-5);
   EXPECT_NEAR(res.ptr<float>(0,0)[2], 72.800f, 1e-5);
}

TEST_F(MyImageProcessingTest, test_get_patch_nearest_neighbor)
{
   cv::Mat patch1, patch2, patch3, patch;
   float x = 3.6f;
   float y = 4.5f;

   float d1[] = {
         60.f, 63.f, 66.f,
         78.f, 81.f, 84.f,
         96.f, 99.f, 102.f,};
   patch1 = MyImageProcessing::get_patch(m_m1, x-1, y-1, 1, MyImageProcessing::InterpolationMode::E_INTERPOLATION_NN);
   cv::Mat expected1(3, 3, CV_32FC1, d1);
   double diff = cv::norm(patch1 - expected1);
   EXPECT_NEAR(0, diff, 1e-5);
   //
   float d2[] = {
         61.f, 64.f, 67.f,
         79.f, 82.f, 85.f,
         97.f, 100.f, 103.f,};
   patch2 = MyImageProcessing::get_patch(m_m2, x-1, y-1, 1, MyImageProcessing::InterpolationMode::E_INTERPOLATION_NN);
   cv::Mat expected2(3, 3, CV_32FC1, d2);
   diff = cv::norm(patch2 - expected2);
   EXPECT_NEAR(0, diff, 1e-5);
   //
   float d3[] = {
         62.f, 65.f, 68.f,
         80.f, 83.f, 86.f,
         98.f, 101.f, 104.f,};
   patch3 = MyImageProcessing::get_patch(m_m3, x-1, y-1, 1, MyImageProcessing::InterpolationMode::E_INTERPOLATION_NN);
   cv::Mat expected3(3, 3, CV_32FC1, d3);
   diff = cv::norm(patch3 - expected3);
   EXPECT_NEAR(0, diff, 1e-5);
   // multichannel
   patch = MyImageProcessing::get_patch(m_m, x-1, y-1, 1, MyImageProcessing::InterpolationMode::E_INTERPOLATION_NN);
   std::vector<cv::Mat> mv;
   mv.push_back(patch1);
   mv.push_back(patch2);
   mv.push_back(patch3);
   cv::Mat expected;
   cv::merge(mv, expected);

   diff = cv::norm(patch - expected);
   EXPECT_NEAR(0, diff, 1e-5);

   //----------
   // test patchsize 1x1
   patch1 = MyImageProcessing::get_patch(m_m1, x-1, y-1, 0, MyImageProcessing::InterpolationMode::E_INTERPOLATION_NN);
   EXPECT_NEAR(patch1.at<float>(0,0), 81.f, 1e-5);

   patch2 = MyImageProcessing::get_patch(m_m2, x-1, y-1, 0, MyImageProcessing::InterpolationMode::E_INTERPOLATION_NN);
   EXPECT_NEAR(patch2.at<float>(0,0), 82.f, 1e-5);

   patch3 = MyImageProcessing::get_patch(m_m3, x-1, y-1, 0, MyImageProcessing::InterpolationMode::E_INTERPOLATION_NN);
   EXPECT_NEAR(patch3.at<float>(0,0), 83.f, 1e-5);

   patch = MyImageProcessing::get_patch(m_m, x-1, y-1, 0, MyImageProcessing::InterpolationMode::E_INTERPOLATION_NN);
   EXPECT_NEAR(patch.at<cv::Vec3f>(0,0)[0], 81.f, 1e-5);
   EXPECT_NEAR(patch.at<cv::Vec3f>(0,0)[1], 82.f, 1e-5);
   EXPECT_NEAR(patch.at<cv::Vec3f>(0,0)[2], 83.f, 1e-5);
}

TEST_F(MyImageProcessingTest, test_get_patch_bicubic)
{
   cv::Mat patch1, patch2, patch3, patch;
   float x = 3.6f;
   float y = 4.5f;

   float d1[] = {
            49.8f, 52.8f, 55.8f,
            67.8f, 70.8f, 73.8f,
            85.8f, 88.8f, 91.8f,};
   patch1 = MyImageProcessing::get_patch(m_m1, x-1, y-1, 1, MyImageProcessing::InterpolationMode::E_INTERPOLATION_BIBUIC);
   cv::Mat expected1(3, 3, CV_32FC1, d1);
   double diff = cv::norm(patch1 - expected1);
   EXPECT_NEAR(0, diff, 1e-5);
   //---------------------
   float d2[] = {
            50.8f, 53.8f, 56.8f,
            68.8f, 71.8f, 74.8f,
            86.8f, 89.8f, 92.8f,};
   patch2 = MyImageProcessing::get_patch(m_m2, x-1, y-1, 1, MyImageProcessing::InterpolationMode::E_INTERPOLATION_BIBUIC);
   cv::Mat expected2(3, 3, CV_32FC1, d2);
   diff = cv::norm(patch2 - expected2);
   EXPECT_NEAR(0, diff, 1e-5);
   //---------------------
   float d3[] = {
         51.8f, 54.8f, 57.8f,
         69.8f, 72.8f, 75.8f,
         87.8f, 90.8f, 93.8f,};
   patch3 = MyImageProcessing::get_patch(m_m3, x-1, y-1, 1, MyImageProcessing::InterpolationMode::E_INTERPOLATION_BIBUIC);
   cv::Mat expected3(3, 3, CV_32FC1, d3);
   diff = cv::norm(patch3 - expected3);
   EXPECT_NEAR(0, diff, 1e-5);

   //---------------------
   // multichannel
   patch = MyImageProcessing::get_patch(m_m, x-1, y-1, 1, MyImageProcessing::InterpolationMode::E_INTERPOLATION_BIBUIC);
   std::vector<cv::Mat> mv;
   mv.push_back(patch1);
   mv.push_back(patch2);
   mv.push_back(patch3);
   cv::Mat expected;
   cv::merge(mv, expected);

   diff = cv::norm(patch - expected);
   EXPECT_NEAR(0, diff, 1e-5);

   //----------
   // test patchsize 1x1
   patch1 = MyImageProcessing::get_patch(m_m1, x-1, y-1, 0, MyImageProcessing::InterpolationMode::E_INTERPOLATION_BIBUIC);
   EXPECT_NEAR(patch1.at<float>(0,0), 70.8f, 1e-5);

   patch2 = MyImageProcessing::get_patch(m_m2, x-1, y-1, 0, MyImageProcessing::InterpolationMode::E_INTERPOLATION_BIBUIC);
   EXPECT_NEAR(patch2.at<float>(0,0), 71.8f, 1e-5);

   patch3 = MyImageProcessing::get_patch(m_m3, x-1, y-1, 0, MyImageProcessing::InterpolationMode::E_INTERPOLATION_BIBUIC);
   EXPECT_NEAR(patch3.at<float>(0,0), 72.8f, 1e-5);

   patch = MyImageProcessing::get_patch(m_m, x-1, y-1, 0, MyImageProcessing::InterpolationMode::E_INTERPOLATION_BIBUIC);
   EXPECT_NEAR(patch.at<cv::Vec3f>(0,0)[0], 70.8f, 1e-5);
   EXPECT_NEAR(patch.at<cv::Vec3f>(0,0)[1], 71.8f, 1e-5);
   EXPECT_NEAR(patch.at<cv::Vec3f>(0,0)[2], 72.8f, 1e-5);
}
