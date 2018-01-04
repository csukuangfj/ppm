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
#ifndef _DescriptorType_HPP_
#define _DescriptorType_HPP_

#include <string>

enum DescriptorType
{
   E_DESC_TYPE_SIFT                          = 0, //!< sift flow descriptor
   E_DESC_TYPE_RANK_TRANSFORM                = 1, //!< rank transform
   E_DESC_TYPE_CENSUS_TRANSFORM              = 2, //!< census transform
   E_DESC_TYPE_COMPLETE_RANK_TRANSFORM       = 3, //!< complete rank transform
   E_DESC_TYPE_COMPLETE_CENSUS_TRANSFORM     = 4, //!< complete census transform
};

/**
 * Convert descriptor type to a string description.
 *
 * @param type_ Type of the descriptor.
 * @return A string representation of the given type.
 */
std::string descriptor_type_to_string(DescriptorType type_);

#endif //_DescriptorType_HPP_
