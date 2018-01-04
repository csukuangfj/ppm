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

#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>

#include "Cpm.hpp"
#include "OpticalFlowKfj.hpp"

#include "common.hpp"

int main(int argc, char* argv[])
{
   std::vector<std::vector<cv::String> > data{
         {
               cv::String(KFJ_DATA_PATH) + "/yos/yos1.pgm",
               cv::String(KFJ_DATA_PATH) + "/yos/yos2.pgm",
               cv::String(KFJ_DATA_PATH) + "/yos/yos_t.flo",
         }, // 0
         {
               cv::String(KFJ_DATA_PATH) + "/middlebury_flow/Hydrangea/frame10.png",
               cv::String(KFJ_DATA_PATH) + "/middlebury_flow/Hydrangea/frame11.png",
               cv::String(KFJ_DATA_PATH) + "/middlebury_flow/Hydrangea/flow10.flo",
         }, // 1
         {
               cv::String(KFJ_DATA_PATH) + "/middlebury_flow/RubberWhale/frame10.png",
               cv::String(KFJ_DATA_PATH) + "/middlebury_flow/RubberWhale/frame11.png",
               cv::String(KFJ_DATA_PATH) + "/middlebury_flow/RubberWhale/flow10.flo",
         }, // 2
   };

   int index = 0;
   if (argc == 2) index =  atoi(argv[1]);
   std::cout << "index: " << index << std::endl;

   cv::String f1_name = data[index][0];
   cv::String f2_name = data[index][1];
   cv::String gt_name = data[index][2];

   std::cout << "first frame (reference): " << f1_name << std::endl;
   std::cout << "second frame: " << f2_name << std::endl;
   std::cout << "ground truth : " << gt_name << std::endl;
   cv::Mat f1, f2;
   f1 = cv::imread(f1_name, cv::IMREAD_COLOR);
   f2 = cv::imread(f2_name, cv::IMREAD_COLOR);

   cv::Ptr<MatchCost> cost_ptr = MatchCost::create("sad");

   CpmConfig config;
   config.set_grid_space(1);
   config.set_pyramid_ratio(0.75);
   config.set_number_of_pyramid_levels(4);
   config.set_number_of_iterations(8);
   config.set_max_displacement(400);
   config.set_half_patch_size(0);
   config.set_minimum_image_width(15);
   config.set_cross_check(true);

   config.set_verbose(0);

   config.set_descriptor_type(DescriptorType::E_DESC_TYPE_SIFT);
   config.set_pm_property_type(CpmConfig::PmPropertyType::E_PROPERTY_FLOW); // translational

   Cpm cpm;
   cpm.init(f1, f2, config, cost_ptr);

   cpm.get_config().show_parameters();

   cpm.compute_optical_flow();

   cv::Mat u, v;
   cv::Mat flow_img;
   u = cpm.get_u();
   v = cpm.get_v();

   OpticalFlowKfj of;
   if (gt_name.size())
   {
      of.read_ground_truth_from_file(gt_name);
   }

   of.set_estimated_flow(u, v);

   flow_img = of.visualize_estimated_flow(FlowVisualization::E_FLOW_VISULIZATION_MIDDLEBURY, 4);

   cv::imwrite("estimated.png", flow_img);
   of.save_estimated_flow_to_file("estimated.flo");

   if (gt_name.size())
   {
      FlowErrorStatistics stat;
      stat = of.get_estimated_flow_error();

      std::cout << "AAE: " << stat.get_AAE() << std::endl;
      std::cout << "AEE: " << stat.get_AEE() << std::endl;
      std::cout << "ABP: " << 100 * stat.get_ABP() << "%" << std::endl;
      std::cout << "Without post-processing, the estimation is really not satisfactory!\n";
   }

   return 0;
}
