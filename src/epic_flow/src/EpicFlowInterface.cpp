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

#include "EpicFlowInterface.hpp"
#include "MatchCost.hpp"
#include "MyTimer.hpp"
#include "MyImageProcessing.hpp"

void
EpicFlowInterface::run_epic_flow(const cv::Mat &image1_,
                                 const cv::Mat &image2_,
                                 const cv::Mat &matches_,
                                 const EpicFlowConfig &epic_config_)
{
   // this function is used by CpmOriginalEpicFlowWrapper.cpp only

   cv::Mat img1, edges;
   edges = get_edge(image1_, false);

   m_epic_flow_wrapper.set_config(epic_config_);
   m_epic_flow_wrapper.init(image1_, image2_, matches_, edges);

   m_epic_flow_wrapper.call_epic_flow();
}

cv::Mat
EpicFlowInterface::get_edge(const cv::Mat &in_image_, bool thinned)
{
   cv::Mat edge;
   MyImageProcessing::compute_edges(in_image_.clone(), edge, thinned);
   return edge;
}
