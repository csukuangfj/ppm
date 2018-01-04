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
#ifndef __EPICFLOWWRAPPER_HPP__
#define __EPICFLOWWRAPPER_HPP__

#include <opencv2/core.hpp>

#include "EpicFlowConfig.hpp"

class EpicFlowWrapper
{
public:
   EpicFlowWrapper() {}

   EpicFlowWrapper(
         const cv::Mat& f_,
         const cv::Mat& g_,
         const cv::Mat& matches_,
         const cv::Mat& edges_,
         const EpicFlowConfig& config_ = EpicFlowConfig()
   );

   void init(
         const cv::Mat& f_,
         const cv::Mat& g_,
         const cv::Mat& matches_,
         const cv::Mat& edges_,
         const EpicFlowConfig& config_ = EpicFlowConfig()
   );

   EpicFlowConfig get_config() const {return m_config;}
   void set_config(const EpicFlowConfig& config_) {m_config = config_;}

   void call_epic_flow();

   cv::Mat get_u() const {return m_u;}
   cv::Mat get_v() const {return m_v;}

   void set_u(const cv::Mat& u_) {m_u = u_;}
   void set_v(const cv::Mat& v_) {m_v = v_;}

private:
   cv::Mat m_f; //!< first frame (reference frame), CV_32FC3
   cv::Mat m_g; //!< second frame, same size and type with m_f

   cv::Mat m_u; //!< CV_32FC1, optical flow in x-direction (horizontal)
   cv::Mat m_v; //!< CV_32FC1, optical flow in y-direction (vertical)

   cv::Mat m_matches; //!< CV_32FC1 with 4 columns

   cv::Mat m_edges;   //!< CV_32FC1, same size with m_f

   EpicFlowConfig m_config;
};

#endif //__EPICFLOWWRAPPER_HPP__
