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
#ifndef __MYTIMER_HPP__
#define __MYTIMER_HPP__

/**
 * My timer class.
 *
 * @deprecated Use cv::TickMeter instead.
 */
class MyTimer
{
public:
   MyTimer()
     : m_tic(0),
       m_toc(0),
       m_elapsed_millisecond(0)
   {}

   void start();

   void stop();

   void reset() {m_tic = m_toc = m_elapsed_millisecond = 0;}

   double get_ms() const {return m_elapsed_millisecond;}
   double get_s() const {return m_elapsed_millisecond/1000;}
private:
   double m_tic;
   double m_toc;
   double m_elapsed_millisecond;
};

#endif //__MYTIMER_HPP__
