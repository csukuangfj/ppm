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
#ifndef __FLOWFORMAT_HPP__
#define __FLOWFORMAT_HPP__

#include <string>
#include <opencv2/core.hpp>

/**
 * Base class for various flow formats.
 */
class FlowFormat
{
public:
   enum Format
   {
      E_FLOW_FORMAT_F      = 0,     //!< The format used in COPCV
      E_FLOW_FORMAT_FLO    = 1,     //!< Flow format for the Middlebury flow dataset and MPI Sintel
      E_FLOW_FORMAT_KITTI  = 2,     //!< Flow format for KITTI-2012 and KITTI-2015
   };

   virtual ~FlowFormat(){}

   /**
    * Read flow from a given file.
    *
    * @param filename_  [in] Filename of the flow.
    * @param u_         [out] Horizontal displacement with type CV_32FC1
    * @param v_         [out] Vertical displacement with type CV_32FC1
    * @param valid_     [out] Same size as u_ and v_. Its type is CV_8UC1.
    *                         A value 1 indicates the flow is valid.
    *                         A value 0 means the flow is invalid.
    */
   virtual void read_flow(
        const std::string& filename_,
        cv::Mat& u_,
        cv::Mat& v_,
        cv::OutputArray valid_ = cv::noArray()
   ) = 0;

   /**
    * Write flow field to a given file.
    *
    * @param filename_  [in] Filename of the flow.
    * @param u_         [in] Horizontal displacement with type CV_32FC1.
    * @param v_         [in] Vertical displacement with type CV_32FC1. It must have the same size with u_.
    * @param valid_     [in] Same size as u_ and v_. Its type is CV_8UC1.
    *                        A value 1 indicates the flow is valid.
    *                        A value 0 indicates the flow is invalid.
    */
   virtual void write_flow(
        const std::string& filename_,
        const cv::Mat& u_,
        const cv::Mat& v_,
        const cv::Mat& valid_ = cv::Mat()
   ) = 0;

   /**
    * Return an instance of a specific flow format.
    *
    * @param name_ [in] Name of the flow format.
    * Possible values are:
    *  - "f", "F", which indicates FlowFormat::E_FLOW_FORMAT_F
    *  - "flo", "FLOW", which indicates FlowFormat::E_FLOW_FORMAT_FLO
    *  - "kitti", "KITTI", "png", which indicates FlowFormat::E_FLOW_FORMAT_KITTI
    *
    * @return A pointer to the specified flow format.
    */
   static cv::Ptr<FlowFormat> create(const std::string& name_);

   /**
    * Return an instance of a specific flow format.
    * @param format_ [in] Type of the flow format.
    * @return A pointer to the specified flow format.
    */
   static cv::Ptr<FlowFormat> create(Format format_);
};

#endif //__FLOWFORMAT_HPP__
