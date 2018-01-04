#include <iostream>
#include <stdio.h>
#include <gtest/gtest.h>

#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>

#include "FlowFormat.hpp"
#include "FlowVisualization.hpp"
#include "MyTools.hpp"
#include "common.hpp"

class FlowFormatTest : public ::testing::Test
{
public:
   virtual void SetUp() override
   {
      float range = 100.0f;
      int rows = 10;
      int cols = 10;
      m_u.create(rows, cols, CV_32FC1);
      m_v.create(rows, cols, CV_32FC1);

      cv::randu(m_u, -range, range);
      cv::randu(m_v, -range, range);

      m_vis = FlowVisualization::create(FlowVisualization::Format::E_FLOW_VISULIZATION_MIDDLEBURY);
   }

   cv::Ptr<FlowFormat> m_format;
   cv::Ptr<FlowVisualization> m_vis;
   cv::Mat m_u;
   cv::Mat m_v;
};

TEST_F(FlowFormatTest, test_FlowFormat_F_read_write)
{
   m_format = FlowFormat::create("f");
   EXPECT_EQ(m_format.empty(), false);

   cv::String filename = "a.F";
   m_format->write_flow(filename, m_u, m_v);

   cv::Mat u1, v1;
   m_format->read_flow(filename, u1, v1);

   double diff = cv::norm(m_u - u1);
   EXPECT_NEAR(diff, 0, 1e-10);

   diff = cv::norm(m_v - v1);
   EXPECT_NEAR(diff, 0, 1e-10);

   // test read file
   cv::String yos_filename = cv::String(KFJ_DATA_PATH) + "/yos/yos_t.F";
   if (!MyTools::file_exists(yos_filename))
   {
      printf("File '%s' does not exist! Skip it!\n", yos_filename.c_str());
      return;
   }

   m_format = FlowFormat::create(yos_filename);
   m_format->read_flow(yos_filename, u1, v1);
   m_format->write_flow(filename, u1, v1);

   cv::Mat u2, v2;
   m_format->read_flow(filename, u2, v2);

   diff = cv::norm(u1 - u2);
   EXPECT_NEAR(diff, 0, 1e-10);

   diff = cv::norm(v1 - v2);
   EXPECT_NEAR(diff, 0, 1e-10);

#if 0
   cv::Mat img;
   m_vis->flow_to_image(u2, v2, img);

   cv::namedWindow("vis-yos-F", 0);
   cv::imshow("vis-yos-F", img);
   cv::waitKey(0);
#endif
}

TEST_F(FlowFormatTest, test_FlowFormat_FLO_read_write)
{
   m_format = FlowFormat::create("flo");
   EXPECT_EQ(m_format.empty(), false);

   cv::String filename = "a.flo";
   m_format->write_flow(filename, m_u, m_v);

   cv::Mat u1, v1;
   m_format->read_flow(filename, u1, v1);

   double diff = cv::norm(m_u - u1);
   EXPECT_NEAR(diff, 0, 1e-10);

   diff = cv::norm(m_v - v1);
   EXPECT_NEAR(diff, 0, 1e-10);

   // test read file
   cv::String yos_filename = cv::String(KFJ_DATA_PATH) + "/yos/yos_t.flo";
   if (!MyTools::file_exists(yos_filename))
   {
      printf("File '%s' does not exist! Skip it!\n", yos_filename.c_str());
      return;
   }

   m_format = FlowFormat::create(yos_filename);
   m_format->read_flow(yos_filename, u1, v1);
   m_format->write_flow(filename, u1, v1);

   cv::Mat u2, v2;
   m_format->read_flow(filename, u2, v2);

   diff = cv::norm(u1 - u2);
   EXPECT_NEAR(diff, 0, 1e-10);

   diff = cv::norm(v1 - v2);
   EXPECT_NEAR(diff, 0, 1e-10);

#if 0
   cv::Mat img;
   m_vis->flow_to_image(u2, v2, img);

   cv::namedWindow("vis-yos-flo", 0);
   cv::imshow("vis-yos-flo", img);
   cv::waitKey(0);
#endif
}

TEST_F(FlowFormatTest, test_FlowFormat_KITTI_read_write)
{
   m_format = FlowFormat::create("png");
   EXPECT_EQ(m_format.empty(), false);

   cv::String filename = "a.png";
   m_format->write_flow(filename, m_u, m_v);

   cv::Mat u1, v1;
   m_format->read_flow(filename, u1, v1);

   double diff = cv::norm(m_u - u1);
   EXPECT_NEAR(diff, 0, 1);

   diff = cv::norm(m_v - v1);
   EXPECT_NEAR(diff, 0, 1);

   // test read file
   cv::String kitti_filename = cv::String(KFJ_DATA_PATH) + "/kitti2012/flow_noc/000010_10.png";
   if (!MyTools::file_exists(kitti_filename))
   {
      printf("File '%s' does not exist! Skip it!\n", kitti_filename.c_str());
      return;
   }

   cv::Mat valid;

   m_format = FlowFormat::create(kitti_filename);
   m_format->read_flow(kitti_filename, u1, v1, valid);
   m_format->write_flow(filename, u1, v1, valid);

   cv::Mat u2, v2;
   m_format->read_flow(filename, u2, v2);

   diff = cv::norm(u1 - u2);
   EXPECT_NEAR(diff, 0, 1e-10);

   diff = cv::norm(v1 - v2);
   EXPECT_NEAR(diff, 0, 1e-10);

#if 0
   cv::Mat img;
   m_vis->flow_to_image(u2, v2, img);

   cv::namedWindow("vis-yos-flo", 0);
   cv::imshow("vis-yos-flo", img);
   cv::waitKey(0);
#endif

}

TEST_F(FlowFormatTest, test_FlowFormat_read_write)
{
   cv::String filename_F = cv::String(KFJ_DATA_PATH) + "/yos/yos_t.F";
   cv::String filename_Flo = cv::String(KFJ_DATA_PATH) + "/yos/yos_t.flo";
   cv::String filename_png = cv::String(KFJ_DATA_PATH) + "/kitti2012/flow_noc/000010_10.png";

   if (!MyTools::file_exists(filename_F))
   {
      printf("File '%s' does not exist. Skip it!\n", filename_F.c_str());
      return;
   }

   if (!MyTools::file_exists(filename_Flo))
   {
      printf("File '%s' does not exist. Skip it!\n", filename_Flo.c_str());
      return;
   }

   if (!MyTools::file_exists(filename_png))
   {
      printf("File '%s' does not exist. Skip it!\n", filename_png.c_str());
      return;
   }
   auto format_F = FlowFormat::create(filename_F);
   auto format_flo = FlowFormat::create(filename_Flo);
   auto format_png = FlowFormat::create(filename_png);

   cv::String test_filename_F = "a.F";
   cv::String test_filename_Flo = "a.flo";
   cv::String test_filename_png = "a.png";

   cv::Mat u1, v1;
   cv::Mat u2, v2;
   double diff;

   //==============================
   // F -> flo, png
   //------------------------------
   format_F->read_flow(filename_F, u1, v1);
   format_flo->write_flow(test_filename_Flo, u1, v1);
   format_flo->read_flow(test_filename_Flo, u2, v2);

   diff = cv::norm(u1 - u2);
   EXPECT_NEAR(diff, 0, 1e-10);
   diff = cv::norm(v1 - v2);
   EXPECT_NEAR(diff, 0, 1e-10);

   format_F->read_flow(filename_F, u1, v1);
   format_png->write_flow(test_filename_png, u1, v1);
   format_png->read_flow(test_filename_png, u2, v2);

   diff = cv::norm(u1 - u2);
   EXPECT_NEAR(diff, 0, 10); // there is data loss if the flow field is saved in PNG format!
   diff = cv::norm(v1 - v2);
   EXPECT_NEAR(diff, 0, 10);

   //==============================
   // flo -> F, png
   //------------------------------
   format_flo->read_flow(filename_Flo, u1, v1);
   format_F->write_flow(test_filename_F, u1, v1);
   format_F->read_flow(test_filename_F, u2, v2);

   diff = cv::norm(u1 - u2);
   EXPECT_NEAR(diff, 0, 1e-10);
   diff = cv::norm(v1 - v2);
   EXPECT_NEAR(diff, 0, 1e-10);

   format_flo->read_flow(filename_Flo, u1, v1);
   format_png->write_flow(test_filename_png, u1, v1);
   format_png->read_flow(test_filename_png, u2, v2);

   diff = cv::norm(u1 - u2);
   EXPECT_NEAR(diff, 0, 10); // there is data loss if the flow field is saved in PNG format!
   diff = cv::norm(v1 - v2);
   EXPECT_NEAR(diff, 0, 10);

   //==============================
   // png -> F, flo
   //------------------------------
   format_png->read_flow(filename_png, u1, v1);
   format_F->write_flow(test_filename_F, u1, v1);
   format_F->read_flow(test_filename_F, u2, v2);

   diff = cv::norm(u1 - u2);
   EXPECT_NEAR(diff, 0, 1e-10);
   diff = cv::norm(v1 - v2);
   EXPECT_NEAR(diff, 0, 1e-10);

   format_png->read_flow(filename_png, u1, v1);
   format_flo->write_flow(test_filename_Flo, u1, v1);
   format_flo->read_flow(test_filename_Flo, u2, v2);

   diff = cv::norm(u1 - u2);
   EXPECT_NEAR(diff, 0, 1e-10);
   diff = cv::norm(v1 - v2);
   EXPECT_NEAR(diff, 0, 1e-10);
}
