// This program is free software: you can use, modify and/or redistribute it
// under the terms of the simplified BSD License. You should have received a
// copy of this license along this program. If not, see
// <http://www.opensource.org/licenses/bsd-license.html>.
//
// Copyright (C) 2012, Javier Sánchez Pérez <jsanchez@dis.ulpgc.es>
// All rights reserved.

// copied from http://www.ipol.im/pub/art/2013/21/
// the original file is named as gaussian.h

#include <cmath>
#include <iostream>

#include "gaussian.hpp"

/**
 * kfj:
 *   This is an in-place operation. The input image
 *   is overwritten using the filtered image!
 *
 *  the kernel size is:
 *    (int)(sigma*precision + 1) * 2 + 1
 *
 *  bc and precsion: the default value is good enough!
 *
 *  bc:
 *    0, the boundary is filled by 0s
 *    1, the boundary is dcb|abcdefg|fed, i.e., reflected
 *    2, the boundary is periodical,    efg|abcdefg|abc
 *  ------------------------------------------------------------
 *
 * Convolution with a Gaussian
 *
 */
void
gaussian(
    float *I,             //input/output image
    const int xdim,       //image width, kfj: i.e.,  nx
    const int ydim,       //image height, kfj: i.e., ny
    const double sigma,   //Gaussian sigma
    const int bc /*=1*/,       //boundary condition
    const int precision/*=5*/ //defines the size of the window
)
{
    int i, j, k;

    const double den  = 2*sigma*sigma;
    const int   size = (int) (precision * sigma) + 1 ;
    const int   bdx  = xdim + size;
    const int   bdy  = ydim + size;

    if ( bc && size > xdim ) {
	std::cerr << "GaussianSmooth: sigma too large for this bc\n" << std::endl;
	throw 1;
    }

    // compute the coefficients of the 1D convolution kernel
    double *B = new double[size];
    for(int i = 0; i < size; i++)
	B[i] = 1 / (sigma * sqrt(2.0 * 3.1415926)) * exp(-i * i / den);

    double norm = 0;

    // normalize the 1D convolution kernel
    for(int i = 0; i < size; i++)
	norm += B[i];

    norm *= 2;

    norm -= B[0];

    for(int i = 0; i < size; i++)
	B[i] /= norm;

    // convolution of each line of the input image
    double *R = new double[size + xdim + size];
    for ( k = 0; k < ydim; k ++ )
    {
	for (i = size; i < bdx; i++)
	    R[i] = I[k * xdim + i - size];

	switch ( bc )
	{
	    case 0:   // Dirichlet boundary conditions

		for( i = 0, j = bdx; i < size; i++,j++) R[i] = R[j] = 0; break;

	    case 1:   // Reflecting boundary conditions   kfj:  dcb|abcdefg|fed

		for(i = 0, j = bdx; i < size; i++, j++) 
		{
		    R[i] = I[k * xdim + size-i];
		    R[j] = I[k * xdim + xdim-i-1];
		}
		break;

	    case 2:   // Periodic boundary conditions

		for(i=0,j=bdx;i<size;i++,j++) 
		{
		  R[i] = I[k * xdim + xdim-size+i];
		  R[j] = I[k * xdim + i];
		}
		break;
		default:
			printf("Error in gaussian()! Unknown code: %d\n", bc); // TODO: print file name, line number
			break;
	}

	for ( i=size; i<bdx; i++ )
	{

	    double sum = B[0] * R[i];

	    for ( int j = 1; j < size; j ++ )
	      sum += B[j] * ( R[i-j] + R[i+j] );

	    I[k * xdim + i - size] = sum;
	}
    }

    // convolution of each column of the input image
    double *T = new double[size + ydim + size];
    for ( k = 0; k < xdim; k ++ )
    {
	for ( i=size; i<bdy; i++ ) T[i] = I[(i - size) * xdim + k];

	switch ( bc )
	{
	    case 0:   // Dirichlet boundary conditions

		for ( i = 0, j = bdy; i < size; i++, j++) T[i] = T[j] = 0; break;

	    case 1:   // Reflecting boundary conditions

		for(i=0,j=bdy;i<size;i++,j++) 
		{
		    T[i] = I[(size-i) * xdim + k];
		    T[j] = I[(ydim-i-1) * xdim + k];
		}
		break;

	    case 2:   // Periodic boundary conditions

		for(i=0,j=bdx;i<size;i++,j++) 
		{
		    T[i] = I[(ydim-size+i) * xdim + k];
		    T[j] = I[i * xdim + k];
		}
		break;
      default:
         printf("Error in gaussian()! Unknown code: %d\n", bc); // TODO: print file name, line number
         break;
	}

	for ( i=size;i<bdy; i++ )
	{
	    double sum = B[0] * T[i];

	    for ( j = 1; j < size; j ++ )
		sum += B[j] * (T[i-j] + T[i+j]);

	    I[(i - size) * xdim + k] = sum;
	}
    }

    delete []B;
    delete []R;
    delete []T;
}




/**
 * kfj: out has to be pre-allocated.
 *
 * Convolution with a Gaussian
 *
 */
void gaussian(
    const float *in,      //input image
    float *out,           //output image
    const int xdim,       //image width
    const int ydim,       //image height
    const double sigma,   //Gaussian sigma
    const int bc/*=1*/,       //boundary condition
    const int precision/*=5*/ //defines the size of the window
)
{
    for(int i = 0; i < xdim*ydim; i++) out[i] = in[i];

    gaussian(out, xdim, ydim, sigma, bc, precision);
}
