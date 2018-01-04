#ifndef _gaussian_HPP_
#define _gaussian_HPP_

/**
 * In-place gaussian filtering.
 *
 * @param I         [in,out] Input and ouput image
 * @param xdim      [in] Image width
 * @param ydim      [in] Image height
 * @param sigma     [in] Sigma of the Gaussian kernel
 * @param bc        [in] Bournday handling code. The default value is enough
 * @param precision [in] The kernel size is (int(precision*sigma)+1)*2+1
 */
void gaussian(
      float *I,             //input/output image
      const int xdim,       //image width, kfj: i.e.,  nx
      const int ydim,       //image height, kfj: i.e., ny
      const double sigma,   //Gaussian sigma
      const int bc=1,       //boundary condition
      const int precision=5 //defines the size of the window
);


/**
 * Gaussian filtering.
 *
 * @param in        [in] Input image.
 * @param out       [out] Output image. The memory should be pre-allocated before calling this function.
 * @param xdim      [in] Image width.
 * @param ydim      [in] Image height.
 * @param sigma     [in] Gaussian sigma.
 * @param bc        [in] Use the default value 1.
 * @param precision [in] The kernel size is ((int)precision*sigma+1) * 2 + 1
 */
void gaussian(
      const float *in,      //input image
      float *out,           //output image
      const int xdim,       //image width
      const int ydim,       //image height
      const double sigma,   //Gaussian sigma
      const int bc=1,       //boundary condition
      const int precision=5 //defines the size of the window
);

#endif //_gaussian_HPP_
