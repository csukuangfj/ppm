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
#ifndef __FlowFormat_KITTI_HPP__
#define __FlowFormat_KITTI_HPP__

#include <opencv2/core.hpp>
#include "FlowFormat.hpp"

/**
 * Read KITTI-2012 and KITTI-2015 flow in PNG format.
 *
 * The input PNG is of type CV_16UC3.
 */
class FlowFormat_KITTI : public FlowFormat
{
public:
   virtual void read_flow(
        const std::string& filename_,
        cv::Mat& u_,
        cv::Mat& v_,
        cv::OutputArray valid_ = cv::noArray()
   ) override;

   virtual void write_flow(
        const std::string& filename_,
        const cv::Mat& u_,
        const cv::Mat& v_,
        const cv::Mat& valid_ = cv::Mat()
   ) override;
};

#endif //__FlowFormat_KITTI_HPP__
