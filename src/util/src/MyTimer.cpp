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
#include <opencv2/core.hpp>

#include "MyTimer.hpp"

void
MyTimer::start()
{
   m_elapsed_millisecond = 0;
   m_tic = (double)cv::getTickCount();
}

void
MyTimer::stop()
{
   m_toc = (double)cv::getTickCount();
   m_elapsed_millisecond = 1000 * (m_toc - m_tic) / cv::getTickFrequency();
}
