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
#ifndef __FLOWVISUALIZATION_FLO_HPP__
#define __FLOWVISUALIZATION_FLO_HPP__

#include <opencv2/core.hpp>
#include "FlowVisualization.hpp"

/**
 * Visualize the flow field using the Middlebury flow color code.
 */
class FlowVisualization_FLO : public FlowVisualization
{
public:
   virtual void flow_to_image(
        const cv::Mat& u_,
        const cv::Mat& v_,
        cv::Mat& image_,
        int max_disp_,
        const cv::Mat& mask_ = cv::Mat()
   ) override;

   // It overloads the parent function to show the measurement unit inside the color wheel
   virtual void get_standard_color_wheel(cv::Mat& image_) override;
};

#endif //__FLOWVISUALIZATION_FLO_HPP__
