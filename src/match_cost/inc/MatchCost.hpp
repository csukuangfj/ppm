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
#ifndef _MATCHCOST_HPP_
#define _MATCHCOST_HPP_

#include <opencv2/core.hpp>

enum MatchCostType
{
   E_COST_TYPE_SSD            = 0, //!< sum of squared distances
   E_COST_TYPE_SAD            = 1, //!< sum of absolute distances
   E_COST_TYPE_HAMMING        = 2, //!< hamming distances
};

/**
 * Base class for various matching costs
 */
class MatchCost
{
public:
   virtual ~MatchCost() {}

   /** Compute the match cost between two frames given
    *  the positions of the two pixels.
    *
    * @param image1  The first frame (2-D), any type supported by cv::norm()
    * @param image2  The second frame
    * @param pixel1  Pixel in the first frame
    * @param pixel2  Pixel in the second frame
    * @param half_patch_size Half patch_size

    * @return Matching cost between the two patches
    *
    */
   virtual double compute_cost(
         const cv::Mat& image1_,
         const cv::Mat& image2_,
         const cv::Point& pixel1_,
         const cv::Point& pixel2_,
         int half_patch_size_
   ) = 0;

   /**
    * Compute cost of two images.
    * The two images should have the same size.
    *
    * @param image1_ Image 1.
    * @param image2_ Image 2.
    * @return
    */
   virtual double compute_cost(
         const cv::Mat& image1_,
         const cv::Mat& image2_
   ) = 0;

   /**
    * Get the name of the cost.
    * @return  Name of the cost.
    */
   virtual cv::String get_name() const = 0;

   /**
    * Create an instance of the specified matching cost.
    * @param name_  Name of the matching cost.
    * @return  An instance of the specified cost type.
    */
   static cv::Ptr<MatchCost> create(const cv::String& name_);

   /**
    * Create an instance of the specified matching cost.
    * @param type_  Type of the matching cost.
    * @return  An instance of the specified cost type.
    */
   static cv::Ptr<MatchCost> create(MatchCostType type_);
};

/** Convert the matching cost type to string for debug output.
 *
 * @param type_ Matching cost type.
 * @return  String representation of the given type.
 */
std::string match_cost_type_to_string(MatchCostType type_);

#endif //_MATCHCOST_HPP_
