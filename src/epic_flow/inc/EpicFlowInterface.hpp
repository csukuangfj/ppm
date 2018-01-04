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
#ifndef _EpicFlowInterface_HPP_
#define _EpicFlowInterface_HPP_

#include <opencv2/core.hpp>

#include "EpicFlowWrapper.hpp"

class EpicFlowConfig;

class EpicFlowInterface
{
public:

   /**
    * This function is used by CpmOriginalEpicFlowWrapper
    *
    * @param image1_      [in] CV_32FC3, un-normalized image.
    * @param image2_      [in] Same as image1_
    * @param matches_     [in] CV_32FC1, 4 columns with order (x1,y1,x2,y2)
    * @param epic_config_ [in] Configurations for EpicFlow
    */
   void run_epic_flow(
         const cv::Mat& image1_,
         const cv::Mat& image2_,
         const cv::Mat& matches_,
         const EpicFlowConfig& epic_config_ = EpicFlowConfig()
   );

   cv::Mat get_u() const {return m_epic_flow_wrapper.get_u();}
   cv::Mat get_v() const {return m_epic_flow_wrapper.get_v();}

   /**
    * Get the image edge used in the interpolation.
    *
    * It uses SED to compute the edge.
    *
    * @param in_image_   [in]  CV_32FC3, un-normalized
    * @param thinned     [in]  true to return a thinned edge
    * @return
    */
   static cv::Mat get_edge(const cv::Mat& in_image_, bool thinned = false);

protected:
   EpicFlowWrapper m_epic_flow_wrapper;
};

#endif //_EpicFlowInterface_HPP_
