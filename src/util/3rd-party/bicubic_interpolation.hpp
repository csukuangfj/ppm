#ifndef _bicubic_interpolation_HPP_
#define _bicubic_interpolation_HPP_

/**
 * Interpolate a point.
 *
 * @param input         [in] Input image
 * @param uu            [in] x-coordinate to be interpolated
 * @param vv            [in] y-coordinate to be interpolated
 * @param nx            [in] Image width
 * @param ny            [in] Image height
 * @param border_out    [in] true to fill the borders with 0s
 * @return
 */
float bicubic_interpolation(
      const float *input,             //image to be interpolated
      const float  uu,                //x component of the vector field,
      const float  vv,                //y component of the vector field, kfj: i.e., (uu, vv) is the point to be interpolated
      const int    nx,                //width of the image
      const int    ny,                //height of the image
      const bool   border_out = false //if true, put zeros outside the region
);

/**
 * Interpolate an image.
 *
 * @param input         [in] input image
 * @param u             [in] x-coordinates to be interpolated
 * @param v             [in] y-coordinates to be interpolated (u,v) are the coordinate pairs
 * @param output        [in] output image
 * @param nx            [in] Image width, i.e., number of columns
 * @param ny            [in] Image height, i.e., number of rows
 * @param border_out    [in] true to fill the pixels out of borders as 0s
 *                           false to process the borders by the defined methods
 */
void bicubic_interpolation(
      const float *input,             //image to be warped
      const float *u,                 //x component of the vector field
      const float *v,                 //y component of the vector field
      float       *output,            //warped output image with bicubic interpolation
      const int    nx,                //width of the image
      const int    ny,                //height of the image
      bool         border_out = false //if true, put zeros outside the region
);

#endif //_bicubic_interpolation_HPP_
