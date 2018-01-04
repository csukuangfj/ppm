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
#ifndef __EPICFLOWCONFIG_HPP__
#define __EPICFLOWCONFIG_HPP__

#include <string>

struct epic_params_s;
struct variational_params_s;

class EpicFlowConfig
{
public:
   enum DatabaseModel
   {
      E_SINTEL = 0,   // 0
      E_KITTI,        // 1
      E_MIDDLEBURY,   // 2
   };

   void to_epic_params(struct epic_params_s& params_);

   void to_variational_params(struct variational_params_s& params_);

   void set_database_model(DatabaseModel model_);

   void show_parameters() const;

public:
   EpicFlowConfig();

   //==============================
   // epic params
   //------------------------------
   /**
    * @param method_ NW for Nadaraya-Watson, LA for Locally Weighted Affine
    */
   void set_method(const std::string& method_) {m_method = method_;}
   const std::string& get_method() const {return m_method;}

   float get_saliency_th() const {return m_saliency_th;}
   void set_saliency_th(float saliency_th_) {m_saliency_th = saliency_th_;}

   int get_pref_nn() const {return m_pref_nn;}
   void set_pref_nn(int pref_nn_) {m_pref_nn = pref_nn_;}

   float get_pref_th() const {return m_pref_th;}
   void set_pref_th(float pref_th_) {m_pref_th = pref_th_;}

   int get_nn() const {return m_nn;}
   void set_nn(int nn_) {m_nn = nn_;}

   float get_coef_kernel() const {return m_coef_kernel;}
   void set_coef_kernel(float coef_kernel_) {m_coef_kernel = coef_kernel_;}

   float get_euc() const {return m_euc;}
   void set_euc(float euc_) {m_euc = euc_;}

   int get_verbose() const {return m_verbose;}
   void set_verbose(int verbose_) {m_verbose = verbose_;}

   //==============================
   // variational params
   //------------------------------
   float get_alpha() const {return m_alpha;}
   void set_alpha(float alpha_) {m_alpha = alpha_;}

   float get_gamma() const {return m_gamma;}
   void set_gamma(float gamma_) {m_gamma = gamma_;}

   float get_delta() const {return m_delta;}
   void set_delta(float delta_) {m_delta = delta_;}

   float get_sigma() const {return m_sigma;}
   void set_sigma(float sigma_) {m_sigma = sigma_;}

   int get_niter_outer() const {return m_niter_outer;}
   void set_niter_outer(float outer_) {m_niter_outer = outer_;}

   int get_niter_inner() const {return m_niter_inner;}
   void set_niter_inner(float inner_) {m_niter_inner = inner_;}

   int get_niter_solver() const {return m_niter_solver;}
   void set_niter_solver(float solver_) {m_niter_solver = solver_;}

   float get_sor_omega() const {return m_sor_omega;}
   void set_sor_omega(float sor_omega_) {m_sor_omega = sor_omega_;}
private:
   //==============================
   // epic params
   //------------------------------
   std::string m_method; //!< NW for Nadaraya-Watson interpolation method,
                         //!< LA for Locally Weighted Affine interpolation method

   float m_saliency_th;  //!< matches coming from pixels with a saliency below this
                         //!< threshold are removed before interpolation

   int m_pref_nn;        //!< number of neighbors for pre-filtering (consistent checking)

   float m_pref_th;      //!< threshold for the first pre-filtering step

   int m_nn;             //!< number of neighbors to consider for the interpolation

   float m_coef_kernel;  //!< coefficient in the sigmoid of the interpolation kernel

   float m_euc;          //!< constant added to the edge cost

   int m_verbose;        //!< verbose mode. 1 to display more log information

   //==============================
   // variational params
   //------------------------------
   float m_alpha;       //!< smoothness weight
   float m_gamma;       //!< gradient constancy assumption weight
   float m_delta;       //!< color constancy assumption weight
   float m_sigma;       //!< pre-smoothing of images
   int m_niter_outer;   //!< number of outer fixed point iterations
   int m_niter_inner;   //!< number of inner fixed point iterations
   int m_niter_solver;  //!< number of solver iterations
   float m_sor_omega;   //!< omega parameter of SOR method
};

#endif //__EPICFLOWCONFIG_HPP__
