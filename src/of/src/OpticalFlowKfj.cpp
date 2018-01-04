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
#include <opencv2/imgcodecs.hpp>
#include "OpticalFlowKfj.hpp"

OpticalFlowKfj::OpticalFlowKfj()
{}

OpticalFlowKfj::~OpticalFlowKfj()
{}

void
OpticalFlowKfj::set_frames(
      const cv::Mat &frame1_,
      const cv::Mat &frame2_
)
{
   CV_Assert(frame1_.size() == frame2_.size());
   CV_Assert(frame1_.type() == frame2_.type());
   CV_Assert(frame1_.type() == CV_32FC3);

   m_frame_1 = frame1_; // shallow copy
   m_frame_2 = frame2_;
}

void
OpticalFlowKfj::read_frames_from_file(
      const cv::String &filename1_,
      const cv::String &filename2_
)
{
   m_frame_1 = cv::imread(filename1_, cv::IMREAD_COLOR);
   m_frame_2 = cv::imread(filename2_, cv::IMREAD_COLOR);

   CV_Assert(m_frame_1.size() == m_frame_2.size());

   m_frame_1.convertTo(m_frame_1, CV_32F);
   m_frame_2.convertTo(m_frame_2, CV_32F);
}

void
OpticalFlowKfj::set_ground_truth(
      const cv::Mat &u_gt_,
      const cv::Mat &v_gt_
)
{
   CV_Assert(u_gt_.size() == v_gt_.size());
   CV_Assert(u_gt_.type() == v_gt_.type());
   CV_Assert(u_gt_.type() == CV_32FC1);
   if (!m_u_est.empty())
   {
      CV_Assert(u_gt_.size() == m_u_est.size());
      CV_Assert(u_gt_.type() == m_u_est.type());
   }

   m_u_gt = u_gt_; // shallow copy
   m_v_gt = v_gt_;
}

void
OpticalFlowKfj::set_estimated_flow(
      const cv::Mat &u_est_,
      const cv::Mat &v_est_
)
{
   CV_Assert(u_est_.size() == v_est_.size());
   CV_Assert(u_est_.type() == v_est_.type());
   CV_Assert(u_est_.type() == CV_32FC1);
   if (!m_u_gt.empty())
   {
      CV_Assert(u_est_.size() == m_u_gt.size());
      CV_Assert(u_est_.type() == m_u_gt.type());
   }

   m_u_est = u_est_; // shallow copy
   m_v_est = v_est_;
}

FlowErrorStatistics
OpticalFlowKfj::get_estimated_flow_error(const cv::_InputArray &mask_) const
{
   FlowErrorStatistics statistics;
   statistics.compute_error_statistics(m_u_gt,
                                       m_v_gt,
                                       m_u_est,
                                       m_v_est,
                                       mask_);
   return statistics;
}

cv::Mat
OpticalFlowKfj::visualize_estimated_flow(
      FlowVisualization::Format format_,
      int max_disp_
) const
{
   cv::Mat vis_image;
   auto vis = FlowVisualization::create(format_);
   vis->flow_to_image(m_u_est, m_v_est, vis_image, max_disp_);
   return vis_image;
}

cv::Mat
OpticalFlowKfj::visualize_ground_truth(
      FlowVisualization::Format format_,
      int max_disp_
) const
{
   cv::Mat vis_image;
   auto vis = FlowVisualization::create(format_);
   vis->flow_to_image(m_u_gt, m_v_gt, vis_image, max_disp_);
   return vis_image;
}

void
OpticalFlowKfj::save_estimated_flow_to_file(
      FlowFormat::Format format_,
      const cv::String &filename_,
      const cv::Mat &valid_
) const
{
   auto flow_format = FlowFormat::create(format_);
   flow_format->write_flow(filename_, m_u_est, m_v_est, valid_);
}

void
OpticalFlowKfj::save_estimated_flow_to_file(
      const cv::String &filename_,
      const cv::Mat &valid_
) const
{
   auto flow_format = FlowFormat::create(filename_);
   flow_format->write_flow(filename_, m_u_est, m_v_est, valid_);
}

void
OpticalFlowKfj::save_ground_truth_to_file(
      FlowFormat::Format format_,
      const cv::String &filename_,
      const cv::Mat &valid_
) const
{
   auto flow_format = FlowFormat::create(format_);
   flow_format->write_flow(filename_, m_u_gt, m_v_gt, valid_);
}

void
OpticalFlowKfj::read_ground_truth_from_file(
      FlowFormat::Format format_,
      const cv::String &filename_,
      cv::OutputArray valid_
)
{
   auto flow_format = FlowFormat::create(format_);
   flow_format->read_flow(filename_, m_u_gt, m_v_gt, valid_);
}

void
OpticalFlowKfj::read_ground_truth_from_file(
      const cv::String &filename_,
      const cv::_OutputArray &valid_
)
{
   auto flow_format = FlowFormat::create(filename_);
   flow_format->read_flow(filename_, m_u_gt, m_v_gt, valid_);
}

void
OpticalFlowKfj::read_estimated_flow_from_file(
      const cv::String &filename_,
      const cv::_OutputArray &valid_
)
{
   auto flow_format = FlowFormat::create(filename_);
   flow_format->read_flow(filename_, m_u_est, m_v_est, valid_);
}

cv::Mat
OpticalFlowKfj::visualize_flow(
      const cv::String &filename_,
      FlowVisualization::Format format_,
      int max_displacement_
)
{
   auto flow_format = FlowFormat::create(filename_);
   cv::Mat u, v, mask, image;
   flow_format->read_flow(filename_, u, v, mask);
   auto vis = FlowVisualization::create(format_);
   vis->flow_to_image(u, v, image, max_displacement_, mask);
   return image;
}
