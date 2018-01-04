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
#include <iostream>
#include <string.h>
#include <opencv2/core.hpp>

#include "EpicFlowConfig.hpp"
#include "epic.h"
#include "variational.h"

void
EpicFlowConfig::to_epic_params(struct epic_params_s &params_)
{
   memset(params_.method, 0, sizeof(params_.method));
   strncpy(params_.method, m_method.c_str(), 2);

   params_.saliency_th = m_saliency_th;
   params_.pref_nn = m_pref_nn;
   params_.pref_th = m_pref_th;
   params_.nn = m_nn;
   params_.coef_kernel = m_coef_kernel;
   params_.euc = m_euc;
   params_.verbose = m_verbose;
}

void
EpicFlowConfig::set_database_model(DatabaseModel model_)
{
   switch (model_)
   {
      case E_SINTEL:
         m_pref_nn = 25;
         m_nn = 160;
         m_coef_kernel = 1.1f;

         m_niter_outer = 5;
         m_alpha = 1.0f;
         m_gamma = 0.72f;
         m_delta = 0.0f;
         m_sigma = 1.1f;
         break;
      case E_KITTI:
         m_pref_nn = 25;
         m_nn = 160;
         m_coef_kernel = 1.1f;

         m_niter_outer = 2;
         m_alpha = 1.0f;
         m_gamma = 0.77f;
         m_delta = 0.0f;
         m_sigma = 1.7f;
         break;
      case E_MIDDLEBURY:
         m_pref_nn = 15;
         m_nn = 65;
         m_coef_kernel = 0.2f;

         m_niter_outer = 25;
         m_alpha = 1.0f;
         m_gamma = 0.72f;
         m_delta = 0.0f;
         m_sigma = 1.1f;
         break;
      default:
         CV_Assert(false); // cannot happen
         break;
   }
}

void
EpicFlowConfig::to_variational_params(struct variational_params_s &params_)
{
   params_.alpha = m_alpha;
   params_.gamma = m_gamma;
   params_.delta = m_delta;
   params_.sigma = m_sigma;

   params_.niter_outer = m_niter_outer;
   params_.niter_inner = m_niter_inner;
   params_.niter_solver = m_niter_solver;

   params_.sor_omega = m_sor_omega;
}

EpicFlowConfig::EpicFlowConfig()
  : m_method("LA"),              // epic params
    m_saliency_th(0.045f),
    m_pref_nn(25),
    m_pref_th(5.0f),
    m_nn(100),
    m_coef_kernel(0.8f),
    m_euc(0.001f),
    m_verbose(0),

    m_alpha(1.0f),              // variational params
    m_gamma(0.71f),
    m_delta(0.0f),
    m_sigma(1.0f),
    m_niter_outer(5),
    m_niter_inner(1),
    m_niter_solver(30),
    m_sor_omega(1.9f)
{
}

void
EpicFlowConfig::show_parameters() const
{
   std::stringstream ss;

   ss << std::endl
      << "method: " << m_method << std::endl
      << "saliency_th: " << m_saliency_th << std::endl
      << "pref_nn: " << m_pref_nn << std::endl
      << "pref_th: " << m_pref_th<< std::endl
      << "nn: " << m_nn << std::endl
      << "coef_kernel: " << m_coef_kernel << std::endl
      << "euc: " << m_euc << std::endl
      << "verbose: " << (m_verbose?"true":"false") << std::endl
      << "---------------------------------------" << std::endl
      << "alpha: " << m_alpha << std::endl
      << "gamma: " << m_gamma << std::endl
      << "delta: " << m_delta << std::endl
      << "sigma: " << m_sigma << std::endl
      << "niter_outer: " << m_niter_outer << std::endl
      << "niter_inner: " << m_niter_inner<< std::endl
      << "niter_solver: " << m_niter_solver<< std::endl
      << "sor_omega: " << m_sor_omega<< std::endl
      << std::endl
      ;

   std::cout << ss.str();
}
