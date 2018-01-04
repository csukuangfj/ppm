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
#ifndef _OpticalFlowKfj_HPP_
#define _OpticalFlowKfj_HPP_

#include <opencv2/core.hpp>

#include "FlowErrorStatistics.hpp"
#include "FlowVisualization.hpp"
#include "FlowFormat.hpp"

/**
 * Every class that estimates the optical flow
 * should be a subclass of this class.
 */
class OpticalFlowKfj
{
public:
   /**
    * Default constructor function.
    * Do nothing.
    */
   OpticalFlowKfj();

   /**
    * Virtual destructor.
    * Do nothing.
    */
   virtual ~OpticalFlowKfj();

   /**
    * Set the frames.
    * @param frame1_    [in] CV_32FC3
    * @param frame2_    [in] CV_32FC3
    */
   void set_frames(
         const cv::Mat& frame1_,
         const cv::Mat& frame2_
   );

   void read_frames_from_file(
         const cv::String& filename1_,
         const cv::String& filename2_
   );

   /**
    * Set the ground truth flow.
    * @param u_gt_      [in] CV_32FC1
    * @param v_gt_      [in] CV_32FC1
    */
   void set_ground_truth(
         const cv::Mat& u_gt_,
         const cv::Mat& v_gt_
   );

   bool has_ground_truth() const {return !m_u_gt.empty();};

   /**
    * Set the estimated flow.
    * @param u_est_     [in] CV_32FC1
    * @param v_est_     [in] CV_32FC1
    */
   void set_estimated_flow(
         const cv::Mat& u_est_,
         const cv::Mat& v_est_
   );

   /**
    * Compute the estimated flow error. If mask is present, then
    * it computes the error only in the regions where
    * the mask is not zero. If mask is not present,
    * then computes the error over the whole region.
    *
    * @param mask_ [in] CV_8UC1 and the same size with m_u_gt.
    *
    * @return Estimated flow errors.
    */
   FlowErrorStatistics get_estimated_flow_error(cv::InputArray mask_ = cv::noArray()) const;

   /**
    * Visualize the estimated flow.
    *
    * @param format_    [in] Visualization format.
    * @param max_disp_  [in] Maximum displacement. A negative number means
    *                        to use the maximum magnitude of the current flow.
    *
    * @return  CV_8UC3. An image for visualization.
    */
   cv::Mat visualize_estimated_flow(
         FlowVisualization::Format format_ = FlowVisualization::E_FLOW_VISULIZATION_MIDDLEBURY,
         int max_disp_ = -1
   ) const;

   /**
    * Visualize the ground truth.
    *
    * @param format_    [in] Visualization format.
    * @param max_disp_  [in] Maximum displacement. A negative number means
    *                        to use the maximum magnitude of the current flow.
    *
    * @return CV_8UC3. An image for visualization.
    */
   cv::Mat visualize_ground_truth(
         FlowVisualization::Format format_ = FlowVisualization::E_FLOW_VISULIZATION_MIDDLEBURY,
         int max_disp_ = -1
   ) const;

   /**
    * Save the estimated flow to file.
    * @param format_    [in] Flow format to be saved.
    * @param filename_  [in] Name of the file.
    * @param valid_     [in] CV_8UC1, same size as the m_u_gt.
    */
   void save_estimated_flow_to_file(
         FlowFormat::Format format_,
         const cv::String& filename_,
         const cv::Mat& valid_ = cv::Mat()
   ) const;

   void save_estimated_flow_to_file(
         const cv::String& filename_,
         const cv::Mat& valid_ = cv::Mat()
   ) const;

   /**
    * Save the ground truth to file.
    * @param format_    [in] Flow format to be saved.
    * @param filename_  [in] Name of the file.
    * @param valid_     [in] CV_8UC1, same size as the m_u_gt.
    */
   void save_ground_truth_to_file(
         FlowFormat::Format format_,
         const cv::String& filename_,
         const cv::Mat& valid_ = cv::Mat()
   ) const;

   /**
    * Read ground truth from file.
    * @param format_    [in] Flow format of the file.
    * @param filename_  [in] Name of the file.
    * @param valid_     [out] CV_8UC1. 1-valid, 0-invalid
    */
   void read_ground_truth_from_file(
         FlowFormat::Format format_,
         const cv::String& filename_,
         cv::OutputArray valid_ = cv::noArray()
   );

   void read_ground_truth_from_file(
         const cv::String& filename_,
         cv::OutputArray valid_ = cv::noArray()
   );

   void read_estimated_flow_from_file(
         const cv::String& filename_,
         cv::OutputArray valid_ = cv::noArray()
   );

   static cv::Mat visualize_flow(
         const cv::String& filename_,
         FlowVisualization::Format format_,
         int max_displacment = -1
   );

public:
   // getters
   cv::Mat get_frame_1() const {return m_frame_1;}
   cv::Mat get_frame_2() const {return m_frame_2;}

   cv::Mat get_estimated_flow_u() const {return m_u_est;}
   cv::Mat get_estimated_flow_v() const {return m_v_est;}

   // setters
   void set_frame_1(const cv::Mat& val_) {m_frame_1 = val_;}
   void set_frame_2(const cv::Mat& val_) {m_frame_2 = val_;}

   void set_estimated_flow_u(const cv::Mat& val_) {m_u_est = val_;}
   void set_estimated_flow_v(const cv::Mat& val_) {m_v_est = val_;}

//protected: // protected, so that subclasses could access them
private:
   cv::Mat m_frame_1;  //!< CV_32FC3, first frame, reference frame
   cv::Mat m_frame_2;  //!< CV_32FC3, second frame

   cv::Mat m_u_gt;     //!< CV_32FC1, ground truth flow in the horizontal direction
   cv::Mat m_v_gt;     //!< CV_32FC1, ground truth flow in the vertical direction

   cv::Mat m_u_est;   //!< CV_32FC1, estimated flow in the horizontal direction
   cv::Mat m_v_est;   //!< CV_32FC1, estimated flow in the vertical direction
};

#endif //_OpticalFlowKfj_HPP_
