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
#include <iostream>
#include <stdio.h>
#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>

#include "PatchMatchStereoSlantedConfig.hpp"
#include "PatchMatchStereoSlanted.hpp"

#include "MyTimer.hpp"
#include "MyTools.hpp"
#include "StereoFormat.hpp"
#include "StErrorStatistics.hpp"
#include "common.hpp"

int main()
{
   cv::String seq = "teddy"; // teddy, cones
   cv::String im1_filename, im2_filename;
   PatchMatchStereoSlantedConfig config;

   im1_filename = cv::String(KFJ_DATA_PATH) + "/stereo/" + seq + "/im2.png";
   im2_filename = cv::String(KFJ_DATA_PATH) + "/stereo/" + seq + "/im6.png";

   config.set_number_of_iterations(1);
   config.set_disparity_scale(4);
   config.set_property_type((PatchMatchStereoSlantedConfig::PropertyType)0); // 0-slanted plane
   config.set_max_disparity(60);
   config.set_verbose(true);
   config.set_output_directory("result");
   if (!MyTools::directory_exists("result")) MyTools::create_directory("result");
   config.set_half_patch_size(15);

   printf("im1_filename: %s\n"
          "im2_filename: %s\n"
          "config: %s\n"
          ,
          im1_filename.c_str(),
          im2_filename.c_str(),

          config.to_string().c_str()
   );

   cv::Mat image1, image2;
   image1 = cv::imread(im1_filename, cv::IMREAD_COLOR);
   image2 = cv::imread(im2_filename, cv::IMREAD_COLOR);
   image1.convertTo(image1, CV_32F);
   image2.convertTo(image2, CV_32F);

   MyTimer timer;
   timer.start();
   PatchMatchStereoSlanted pmst;
   pmst.init(image1, image2, config);
   pmst.run_PatchMatch_stereo();
   timer.stop();

   cv::String time_txt = cv::format("echo %.3f > %s/time.txt", config.get_output_directory().c_str(), timer.get_s());
   system(time_txt.c_str());

   cv::String disp0_pfm = cv::format("%s/disp0.pfm", config.get_output_directory().c_str());
   auto pfm_format = StereoFormat::create(disp0_pfm.c_str());
   const cv::Mat& disparity = pmst.get_estimated_disparity_scaled(PatchMatchStereoSlanted::LEFT_VIEW);
   pfm_format->write_disparity_to_file(disp0_pfm.c_str(), disparity);

   // compute error

   cv::String out_filename;
   cv::String gt_filename;
   cv::String mask_all, mask_disc, mask_nonocc;
   cv::String image1_filename, image2_filename;

   out_filename = config.get_output_directory() + "/final-estimation-left.png";

   gt_filename = cv::String(KFJ_DATA_PATH) + "/stereo/" + seq + "/groundtruth.png";
   mask_all = cv::String(KFJ_DATA_PATH) + "/stereo/" + seq + "/all.png";
   mask_disc = cv::String(KFJ_DATA_PATH) + "/stereo/" + seq + "/disc.png";
   mask_nonocc = cv::String(KFJ_DATA_PATH) + "/stereo/" + seq + "/nonocc.png";

   cv::Mat t;
   disparity.convertTo(t, CV_8UC1);
   cv::imwrite(out_filename, t);

   StErrorStatistics stat;
   float threshold = 1;
   stat.evaluate_MiddleburyV2(gt_filename, out_filename, mask_all, mask_disc, mask_nonocc,
                              config.get_disparity_scale(),threshold);
   printf("%s\n", stat.to_string().c_str());

   return 0;
}
