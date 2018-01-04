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
#ifndef __FLOWVISUALIZATION_HPP__
#define __FLOWVISUALIZATION_HPP__

#include <string>
#include <opencv2/core.hpp>

/**
 * Base class for visualizing the flow field.
 */
class FlowVisualization
{
public:
   enum Format
   {
      E_FLOW_VISULIZATION_FLO                = 1,        //!< Middlebury
      E_FLOW_VISULIZATION_MIDDLEBURY         = 2,        //!< Middlebury
      E_FLOW_VISULIZATION_KITTI              = 3,        //!< KITTI-2012 and KITTI-2015
   };

   virtual ~FlowVisualization() {}

   /**
    *  Visualize the flow field.
    *
    * @param u_         [in] Flow field in the horizontal direction, CV_32FC1
    * @param v_         [in] Flow field in the vertical direction, CV_32FC1
    * @param image_     [out] Output image that can be visualized directly, CV_8UC3
    * @param max_disp_  [in] Maximum displacement for visualization. A negative value means to
    *                        use the maximum flow magnitude.
    *
    * @param mask_      [in] CV_8UC1. If it is not empty, then a value 0 means to show this pixel
    *                        in black, otherwise show this pixel using its flow magnitude. It is useful
    *                        for the KITTI dataset and the mask can be considered as the occluded mask.
    *                        Non-zero means the pixel is not occluded.
    */
   virtual void flow_to_image(
        const cv::Mat& u_,
        const cv::Mat& v_,
        cv::Mat& image_,
        int max_disp_ = -1,
        const cv::Mat& mask_ = cv::Mat()
   ) = 0;

   /**
    * Color code for this visualization method.
    *
    * @param image_ [out] CV_8UC3, the standard color wheel for visualization. The background is in black.
    */
   virtual void get_standard_color_wheel(cv::Mat& image_);

   /**
    * Create an instance of the specified visualization scheme.
    *
    * @param name_ [in] Name of the visualization method.
    * Possible values are:
    *  - "copcv", "COPCV", which indicates Format::E_FLOW_VISULIZATION_COPCV
    *  - "flo", "FLO", "Middlebury", which indicates Format::E_FLOW_VISULIZATION_MIDDLEBURY and Format::E_FLOW_VISULIZATION_FLO
    *  - "kitti", "KITTI", which indicates Format::E_FLOW_VISULIZATION_KITTI
    *
    * @return A pointer to the created instance.
    */
   static cv::Ptr<FlowVisualization> create(const std::string& name_);

   /**
    * Create an instance of the specified visualization scheme.
    *
    * @param format_ [in] The visualization format.
    * @return A pointer to the created instance.
    */
   static cv::Ptr<FlowVisualization> create(Format format_);
};

#endif //__FLOWVISUALIZATION_HPP__
