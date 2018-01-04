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
#ifndef __CPM_HPP__
#define __CPM_HPP__

#include <vector>

#include <opencv2/core.hpp>

#include "MatchCost.hpp"
#include "CpmConfig.hpp"


class Cpm
{
public:
   Cpm() {}

   /**
    * @param image1_    [in] First frame, reference frame, depth: CV_8U or CV_32F
    * @param image2_    [in] Second frame, depth: CV_8U or CV_32F
    * @param config_    [in] Configurations
    * @param cost_ptr_  [in] For computing matching cost
    */
   Cpm(
         const cv::Mat& image1_,
         const cv::Mat& image2_,
         const CpmConfig& config_,
         cv::Ptr<MatchCost> cost_ptr_
   );

   /**
    * @param image1_    [in] First frame, reference frame, depth: CV_8U or CV_32F
    * @param image2_    [in] Second frame, depth: CV_8U or CV_32F
    * @param config_    [in] Configurations
    * @param cost_ptr_  [in] For computing matching cost
    */
   void init(
         const cv::Mat& image1_,
         const cv::Mat& image2_,
         const CpmConfig& config_,
         cv::Ptr<MatchCost> cost_ptr_
   );

   //! Run ppm flow
   void compute_optical_flow();

   /**
    * Get the matches of PPM. It can be used as input for Epic flow
    * @param matches_ [out] CV_32FC1.
    *                       Column 0 - x1 in the first frame (i.e, m_f)
    *                       Column 1 - y1 in the first frame (i.e, m_f)
    *                       Column 2 - x2 in the second frame (i.e, m_g)
    *                       Column 3 - y2 in the second frame (i.e, m_g)
    */
   void get_matches(cv::Mat& matches_);

   cv::Mat get_frame1() const {return m_f;}
   cv::Mat get_frame2() const {return m_g;}

   cv::Mat get_u() const {return m_u;}
   cv::Mat get_v() const {return m_v;}

   CpmConfig& get_config() {return m_config;}
   void set_config(const CpmConfig& val_) {m_config = val_;}

protected: // protected for testing
   /**
    * set up the pyramid
    */
   void init_pyramid();

   /**
    * Compute descriptor for each pixel
    * in the pyramid
    */
   void compute_descriptor();

   /**
    * Compute the coordinate of every pixel at each level in the pyramid.
    */
   void init_seeds();

protected:

   void run_patch_match();
   cv::Ptr<MatchCost> m_cost_ptr;

protected: // protected for testing
   int m_num_seeds;        //!< number of seeds
   int m_seeds_per_col;    //!< number of seeds per column
   int m_seeds_per_row;    //!< number of seeds per row
   int m_seed_x_offset;    //!< x coordinate offset of a seed
   int m_seed_y_offset;    //!< y coordinate offset of a seed

protected: // protected for testing
   //****************************************************
   // Intermediate Data members
   //***************************************************

   /**
    * Every element has the following format:
    *  - height: number of seeds
    *  - width: 2. 0-th column: x coordinate of the seed, 1-st column: y coordinate of the seed
    *  - type: CV_32SC1
    */
   std::vector<cv::Mat> m_seeds;

   /**
    * Because seeds are not continuous, we need to keep the information of its neighbors.
    *
    * Index of seed's neighbors
    *
    * CV_32SC1, 8 columns
    */
   cv::Mat m_seed_neighbors;

   /**
    * Pyramid of the raw image m_f.
    *
    *  - Number of elements in the vector: m_num_pyramid_levels
    *  - the 0th element is the raw image, i.e, m_f
    *  - the m_num_pyramid_levels-1 th image is the coarsest image
    */
   std::vector<cv::Mat> m_f_pyramid;

   /**
    * Pyramids of the raw image m_g.
    *
    *  - Number of elements in the vector: m_num_pyramid_levels
    *  - the 0th element is the raw image, i.e, m_g
    *  - the m_num_pyramid_levels-1 th image is the coarsest image
    */
   std::vector<cv::Mat> m_g_pyramid;

   /**
    * descriptors for every pixel in the pyramid
    */
   std::vector<cv::Mat> m_f_pyramid_descriptor;

   /**
    * descriptors for every pixel in the pyramid
    */
   std::vector<cv::Mat> m_g_pyramid_descriptor;

   /**
    * Horizontal flow field of seed pixels at every level.
    *
    * Every element is of type CV_32FC1.
    */
   std::vector<cv::Mat> m_seeds_flow_u;

   /**
    * Vertical flow field of seed pixels at every level.
    *
    * Every element is of type CV_32FC1.
    */
   std::vector<cv::Mat> m_seeds_flow_v;

   std::vector<cv::Mat> m_seeds_flow_cost;

protected: // protected for testing
   //****************************************************
   //  Data members
   //***************************************************
   cv::Mat m_f; //!< image 1, reference frame, raw image,
                //!< depth: CV_8U or CV_32F
   cv::Mat m_g; //!< image 2, same type with m_f

   cv::Mat m_u; //!< flow field in the x direction, CV_32FC1, fields of non-seed pixels are set to 0
   cv::Mat m_v; //!< flow field in the y direction, CV_32FC1, fields of non-seed pixels are set to 0

   CpmConfig m_config;
};

#endif //__CPM_HPP__
