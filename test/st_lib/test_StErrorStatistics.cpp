#include <iostream>
#include <string>
#include <gtest/gtest.h>

#include "StErrorStatistics.hpp"
#include "MyTools.hpp"
#include "common.hpp"

TEST(test_StErrorStatistics, test_evaluate_MiddleburyV2_tsukuba)
{
   cv::String common_dir = cv::String(KFJ_DATA_PATH) + "/stereo";
   cv::String sequence = "tsukuba";

   cv::String estimated_disparity = common_dir + "/" + sequence;

   common_dir = common_dir + "/" + sequence;

   cv::String ground_truth = common_dir + "/" + "groundtruth.png";
   cv::String mask_all = common_dir + "/" + "all.png";
   cv::String mask_disc = common_dir + "/" + "disc.png";
   cv::String mask_nonocc = common_dir + "/" + "nonocc.png";

   // alg117: disparity map for PatchMatch Stereo: slanted support windows
   // http://vision.middlebury.edu/stereo/eval/newEval/imagedirs/tsukuba/results/alg117.png
   estimated_disparity = estimated_disparity + "/" + "alg117.png";

   if (!MyTools::file_exists(ground_truth))
   {
      printf("File '%s' does not exist!\n", ground_truth.c_str());
      return;
   }

   if (!MyTools::file_exists(estimated_disparity))
   {
      printf("File '%s' does not exist!\n", estimated_disparity.c_str());
      return;
   }

   if (!MyTools::file_exists(mask_all))
   {
      printf("File '%s' does not exist!\n", mask_all.c_str());
      return;
   }

   if (!MyTools::file_exists(mask_disc))
   {
      printf("File '%s' does not exist!\n", mask_disc.c_str());
      return;
   }

   if (!MyTools::file_exists(mask_nonocc))
   {
      printf("File '%s' does not exist!\n", mask_nonocc.c_str());
      return;
   }

   float scale = 16;
   float threshold;

   StErrorStatistics stat;

   threshold = 0.5;
   stat.evaluate_MiddleburyV2(ground_truth, estimated_disparity, mask_all, mask_disc, mask_nonocc, scale, threshold);
   EXPECT_NEAR(15.0f, stat.get_nonocc(), 0.1f);
   EXPECT_NEAR(15.4f, stat.get_all(), 0.1f);
   EXPECT_NEAR(20.3f, stat.get_disc(), 0.1f);

   threshold = 0.75;
   stat.evaluate_MiddleburyV2(ground_truth, estimated_disparity, mask_all, mask_disc, mask_nonocc, scale, threshold);
   EXPECT_NEAR(15.0f, stat.get_nonocc(), 0.1f);
   EXPECT_NEAR(15.4f, stat.get_all(), 0.1f);
   EXPECT_NEAR(20.3f, stat.get_disc(), 0.1f);

   threshold = 1;
   stat.evaluate_MiddleburyV2(ground_truth, estimated_disparity, mask_all, mask_disc, mask_nonocc, scale, threshold);
   EXPECT_NEAR(2.09f, stat.get_nonocc(), 0.1f);
   EXPECT_NEAR(2.33f, stat.get_all(), 0.1f);
   EXPECT_NEAR(9.31f, stat.get_disc(), 0.1f);

   threshold = 1.5;
   stat.evaluate_MiddleburyV2(ground_truth, estimated_disparity, mask_all, mask_disc, mask_nonocc, scale, threshold);
   EXPECT_NEAR(2.09f, stat.get_nonocc(), 0.1f);
   EXPECT_NEAR(2.33f, stat.get_all(), 0.1f);
   EXPECT_NEAR(9.31f, stat.get_disc(), 0.1f);

   threshold = 2;
   stat.evaluate_MiddleburyV2(ground_truth, estimated_disparity, mask_all, mask_disc, mask_nonocc, scale, threshold);
   EXPECT_NEAR(1.77f, stat.get_nonocc(), 0.1f);
   EXPECT_NEAR(1.92f, stat.get_all(), 0.1f);
   EXPECT_NEAR(7.85f, stat.get_disc(), 0.1f);
}

TEST(test_StErrorStatistics, test_evaluate_MiddleburyV2_venus)
{
   cv::String common_dir = cv::String(KFJ_DATA_PATH) + "/stereo";
   cv::String sequence = "venus";

   cv::String estimated_disparity = common_dir + "/" + sequence;

   common_dir = common_dir + "/" + sequence;

   cv::String ground_truth = common_dir + "/" + "groundtruth.png";
   cv::String mask_all = common_dir + "/" + "all.png";
   cv::String mask_disc = common_dir + "/" + "disc.png";
   cv::String mask_nonocc = common_dir + "/" + "nonocc.png";

   // alg117: disparity map for PatchMatch Stereo: slanted support windows
   // http://vision.middlebury.edu/stereo/eval/newEval/imagedirs/venus/results/alg117.png
   estimated_disparity = estimated_disparity + "/" + "alg117.png";

   if (!MyTools::file_exists(ground_truth))
   {
      printf("File '%s' does not exist!\n", ground_truth.c_str());
      return;
   }

   if (!MyTools::file_exists(estimated_disparity))
   {
      printf("File '%s' does not exist!\n", estimated_disparity.c_str());
      return;
   }

   if (!MyTools::file_exists(mask_all))
   {
      printf("File '%s' does not exist!\n", mask_all.c_str());
      return;
   }

   if (!MyTools::file_exists(mask_disc))
   {
      printf("File '%s' does not exist!\n", mask_disc.c_str());
      return;
   }

   if (!MyTools::file_exists(mask_nonocc))
   {
      printf("File '%s' does not exist!\n", mask_nonocc.c_str());
      return;
   }

   float scale = 8;
   float threshold;

   StErrorStatistics stat;

   threshold = 0.5;
   stat.evaluate_MiddleburyV2(ground_truth, estimated_disparity, mask_all, mask_disc, mask_nonocc, scale, threshold);
   EXPECT_NEAR(1.00f, stat.get_nonocc(), 0.1f);
   EXPECT_NEAR(1.34f, stat.get_all(), 0.1f);
   EXPECT_NEAR(7.75f, stat.get_disc(), 0.1f);

   threshold = 0.75;
   stat.evaluate_MiddleburyV2(ground_truth, estimated_disparity, mask_all, mask_disc, mask_nonocc, scale, threshold);
   EXPECT_NEAR(0.41f, stat.get_nonocc(), 0.1f);
   EXPECT_NEAR(0.64f, stat.get_all(), 0.1f);
   EXPECT_NEAR(4.38f, stat.get_disc(), 0.1f);

   threshold = 1;
   stat.evaluate_MiddleburyV2(ground_truth, estimated_disparity, mask_all, mask_disc, mask_nonocc, scale, threshold);
   EXPECT_NEAR(0.21f, stat.get_nonocc(), 0.1f);
   EXPECT_NEAR(0.39f, stat.get_all(), 0.1f);
   EXPECT_NEAR(2.62f, stat.get_disc(), 0.1f);

   threshold = 1.5;
   stat.evaluate_MiddleburyV2(ground_truth, estimated_disparity, mask_all, mask_disc, mask_nonocc, scale, threshold);
   EXPECT_NEAR(0.13f, stat.get_nonocc(), 0.1f);
   EXPECT_NEAR(0.24f, stat.get_all(), 0.1f);
   EXPECT_NEAR(1.76f, stat.get_disc(), 0.1f);

   threshold = 2;
   stat.evaluate_MiddleburyV2(ground_truth, estimated_disparity, mask_all, mask_disc, mask_nonocc, scale, threshold);
   EXPECT_NEAR(0.12f, stat.get_nonocc(), 0.1f);
   EXPECT_NEAR(0.20f, stat.get_all(), 0.1f);
   EXPECT_NEAR(1.72f, stat.get_disc(), 0.1f);
}

TEST(test_StErrorStatistics, test_evaluate_MiddleburyV2_teddy)
{
   cv::String common_dir = cv::String(KFJ_DATA_PATH) + "/stereo";
   cv::String sequence = "teddy";

   cv::String estimated_disparity = common_dir + "/" + sequence;

   common_dir = common_dir + "/" + sequence;

   cv::String ground_truth = common_dir + "/" + "groundtruth.png";
   cv::String mask_all = common_dir + "/" + "all.png";
   cv::String mask_disc = common_dir + "/" + "disc.png";
   cv::String mask_nonocc = common_dir + "/" + "nonocc.png";

   // alg117: disparity map for PatchMatch Stereo: slanted support windows
   // http://vision.middlebury.edu/stereo/eval/newEval/imagedirs/teddy/results/alg117.png
   estimated_disparity = estimated_disparity + "/" + "alg117.png";

   if (!MyTools::file_exists(ground_truth))
   {
      printf("File '%s' does not exist!\n", ground_truth.c_str());
      return;
   }

   if (!MyTools::file_exists(estimated_disparity))
   {
      printf("File '%s' does not exist!\n", estimated_disparity.c_str());
      return;
   }

   if (!MyTools::file_exists(mask_all))
   {
      printf("File '%s' does not exist!\n", mask_all.c_str());
      return;
   }

   if (!MyTools::file_exists(mask_disc))
   {
      printf("File '%s' does not exist!\n", mask_disc.c_str());
      return;
   }

   if (!MyTools::file_exists(mask_nonocc))
   {
      printf("File '%s' does not exist!\n", mask_nonocc.c_str());
      return;
   }

   float scale = 4;
   float threshold;

   StErrorStatistics stat;

   threshold = 0.5;
   stat.evaluate_MiddleburyV2(ground_truth, estimated_disparity, mask_all, mask_disc, mask_nonocc, scale, threshold);
   EXPECT_NEAR(5.66f, stat.get_nonocc(), 0.1f);
   EXPECT_NEAR(11.8f, stat.get_all(), 0.1f);
   EXPECT_NEAR(16.5f, stat.get_disc(), 0.1f);

   threshold = 0.75;
   stat.evaluate_MiddleburyV2(ground_truth, estimated_disparity, mask_all, mask_disc, mask_nonocc, scale, threshold);
   EXPECT_NEAR(3.84f, stat.get_nonocc(), 0.1f);
   EXPECT_NEAR(9.48f, stat.get_all(), 0.1f);
   EXPECT_NEAR(12.0f, stat.get_disc(), 0.1f);

   threshold = 1;
   stat.evaluate_MiddleburyV2(ground_truth, estimated_disparity, mask_all, mask_disc, mask_nonocc, scale, threshold);
   EXPECT_NEAR(2.99f, stat.get_nonocc(), 0.1f);
   EXPECT_NEAR(8.16f, stat.get_all(), 0.1f);
   EXPECT_NEAR(9.62f, stat.get_disc(), 0.1f);

   threshold = 1.5;
   stat.evaluate_MiddleburyV2(ground_truth, estimated_disparity, mask_all, mask_disc, mask_nonocc, scale, threshold);
   EXPECT_NEAR(2.28f, stat.get_nonocc(), 0.1f);
   EXPECT_NEAR(6.56f, stat.get_all(), 0.1f);
   EXPECT_NEAR(7.53f, stat.get_disc(), 0.1f);

   threshold = 2;
   stat.evaluate_MiddleburyV2(ground_truth, estimated_disparity, mask_all, mask_disc, mask_nonocc, scale, threshold);
   EXPECT_NEAR(1.80f, stat.get_nonocc(), 0.1f);
   EXPECT_NEAR(5.36f, stat.get_all(), 0.1f);
   EXPECT_NEAR(5.99f, stat.get_disc(), 0.1f);
}

TEST(test_StErrorStatistics, test_evaluate_MiddleburyV2_cones)
{
   cv::String common_dir = cv::String(KFJ_DATA_PATH) + "/stereo";
   cv::String sequence = "cones";

   cv::String estimated_disparity = common_dir + "/" + sequence;

   common_dir = common_dir + "/" + sequence;

   cv::String ground_truth = common_dir + "/" + "groundtruth.png";
   cv::String mask_all = common_dir + "/" + "all.png";
   cv::String mask_disc = common_dir + "/" + "disc.png";
   cv::String mask_nonocc = common_dir + "/" + "nonocc.png";

   // alg117: disparity map for PatchMatch Stereo: slanted support windows
   // http://vision.middlebury.edu/stereo/eval/newEval/imagedirs/cones/results/alg117.png
   estimated_disparity = estimated_disparity + "/" + "alg117.png";

   if (!MyTools::file_exists(ground_truth))
   {
      printf("File '%s' does not exist!\n", ground_truth.c_str());
      return;
   }

   if (!MyTools::file_exists(estimated_disparity))
   {
      printf("File '%s' does not exist!\n", estimated_disparity.c_str());
      return;
   }

   if (!MyTools::file_exists(mask_all))
   {
      printf("File '%s' does not exist!\n", mask_all.c_str());
      return;
   }

   if (!MyTools::file_exists(mask_disc))
   {
      printf("File '%s' does not exist!\n", mask_disc.c_str());
      return;
   }

   if (!MyTools::file_exists(mask_nonocc))
   {
      printf("File '%s' does not exist!\n", mask_nonocc.c_str());
      return;
   }

   float scale = 4;
   float threshold;

   StErrorStatistics stat;

   threshold = 0.5;
   stat.evaluate_MiddleburyV2(ground_truth, estimated_disparity, mask_all, mask_disc, mask_nonocc, scale, threshold);
   EXPECT_NEAR(3.8f, stat.get_nonocc(), 0.1f);
   EXPECT_NEAR(10.2f, stat.get_all(), 0.1f);
   EXPECT_NEAR(10.2f, stat.get_disc(), 0.1f);

   threshold = 0.75;
   stat.evaluate_MiddleburyV2(ground_truth, estimated_disparity, mask_all, mask_disc, mask_nonocc, scale, threshold);
   EXPECT_NEAR(2.81f, stat.get_nonocc(), 0.1f);
   EXPECT_NEAR(8.55f, stat.get_all(), 0.1f);
   EXPECT_NEAR(7.95f, stat.get_disc(), 0.1f);

   threshold = 1;
   stat.evaluate_MiddleburyV2(ground_truth, estimated_disparity, mask_all, mask_disc, mask_nonocc, scale, threshold);
   EXPECT_NEAR(2.47f, stat.get_nonocc(), 0.1f);
   EXPECT_NEAR(7.80f, stat.get_all(), 0.1f);
   EXPECT_NEAR(7.11f, stat.get_disc(), 0.1f);

   threshold = 1.5;
   stat.evaluate_MiddleburyV2(ground_truth, estimated_disparity, mask_all, mask_disc, mask_nonocc, scale, threshold);
   EXPECT_NEAR(2.19f, stat.get_nonocc(), 0.1f);
   EXPECT_NEAR(6.86f, stat.get_all(), 0.1f);
   EXPECT_NEAR(6.46f, stat.get_disc(), 0.1f);

   threshold = 2;
   stat.evaluate_MiddleburyV2(ground_truth, estimated_disparity, mask_all, mask_disc, mask_nonocc, scale, threshold);
   EXPECT_NEAR(1.99f, stat.get_nonocc(), 0.1f);
   EXPECT_NEAR(6.17f, stat.get_all(), 0.1f);
   EXPECT_NEAR(5.93f, stat.get_disc(), 0.1f);
}
