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
#include <sys/stat.h>
#include <unistd.h>

#include "MyTools.hpp"

void
MyTools::concatenate_images(
     const std::vector<cv::Mat>& pyramid_,
     cv::Mat& img_
)
{
   int w = pyramid_[0].cols, h = 0;
   int num_levels = (int)pyramid_.size();

   for (int i = 1; i < num_levels; i++)
   {
      if (i == 1) w += pyramid_[1].cols;
      h += pyramid_[i].rows;
   }
   h = cv::max(pyramid_[0].rows, h);

   img_.create(h, w, pyramid_[0].type());
   img_ = 0;
   pyramid_[0].copyTo(img_(cv::Range(0, pyramid_[0].rows),
                         cv::Range(0, pyramid_[0].cols)));
   int x = pyramid_[0].cols;
   int y = 0;
   for (int i = 1; i < num_levels; i++)
   {
      pyramid_[i].copyTo(img_(cv::Range(y, y+pyramid_[i].rows),
                            cv::Range(x, x+pyramid_[i].cols)));
      y += pyramid_[i].rows;
   }
}

bool
MyTools::file_exists(const std::string &filename_)
{
   struct stat buffer;
   return (stat (filename_.c_str(), &buffer) == 0);
}

bool
MyTools::directory_exists(const std::string &path_)
{
   struct stat buffer;
   return (stat (path_.c_str(), &buffer) == 0);
}

void
MyTools::create_directory(const std::string &path_)
{
   cv::String cmd_string = cv::format("mkdir -pv %s", path_.c_str());
   system(cmd_string.c_str());
}
