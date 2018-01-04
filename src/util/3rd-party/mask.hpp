#ifndef _mask_HPP_
#define _mask_HPP_

void mask3x3(
      const float *input, //input image
      float *output,      //output image
      const int nx,       //image width
      const int ny,       //image height
      const float *mask   //mask to be applied
);

void Dxx(
      const float *I, //input image
      float *Ixx,     //oputput derivative
      const int nx,   //image width
      const int ny    //image height
);

void Dyy(
      const float *I, //input image
      float *Iyy,     //oputput derivative
      const int nx,   //image width
      const int ny    //image height
);

void Dxy(
      const float *I, //input image
      float *Ixy,     //oputput derivative
      const int nx,   //image width
      const int ny    //image height
);

void gradient(
      const float *input, //input image
      float *dx,          //computed x derivative
      float *dy,          //computed y derivative
      const int nx,       //image width
      const int ny        //image height
);

void psi_divergence(
      const float *psi, //robust functional
      float *psi1,      //coefficients of divergence
      float *psi2,      //coefficients of divergence
      float *psi3,      //coefficients of divergence
      float *psi4,      //coefficients of divergence
      const int nx,     //image width
      const int ny      //image height
);

void divergence_u(
      const float *u,    //x component of optical flow
      const float *v,    //y component of optical flow
      const float *psi1, //coefficients of divergence
      const float *psi2, //coefficients of divergence
      const float *psi3, //coefficients of divergence
      const float *psi4, //coefficients of divergence
      float *div_u,      //computed divergence for u
      float *div_v,      //computed divergence for v
      const int nx,      //image width
      const int ny       //image height
);
#endif //_mask_HPP_
