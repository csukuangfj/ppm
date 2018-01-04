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
#ifndef __FlowVisualization_KITTI_HPP__
#define __FlowVisualization_KITTI_HPP__

#include "FlowVisualization.hpp"


/**
 * Visualization of KITTI optical flow.
 * Refer to
 *  - devkit/cpp/io_flow.h in http://kitti.is.tue.mpg.de/kitti/devkit_scene_flow.zip
 *
 */
class FlowVisualization_KITTI : public  FlowVisualization
{
public:
   virtual void flow_to_image(
        const cv::Mat& u_,
        const cv::Mat& v_,
        cv::Mat& image_,
        int max_disp_,
        const cv::Mat& mask_ = cv::Mat()
   ) override;
};

#endif //__FlowVisualization_KITTI_HPP__
