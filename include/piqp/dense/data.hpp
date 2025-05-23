// This file is part of PIQP.
//
// Copyright (c) 2024 EPFL
// Copyright (c) 2022 INRIA
//
// This source code is licensed under the BSD 2-Clause License found in the
// LICENSE file in the root directory of this source tree.

#ifndef PIQP_DENSE_DATA_HPP
#define PIQP_DENSE_DATA_HPP

#include "piqp/fwd.hpp"
#include "piqp/typedefs.hpp"
#include "piqp/dense/model.hpp"

namespace piqp
{

namespace dense
{

template<typename T>
struct Data
{
    isize n; // number of variables
    isize p; // number of equality constraints
    isize m; // number of inequality constraints

    Mat<T> P_utri; // upper triangular part of P
    Mat<T> AT;     // A transpose
    Mat<T> GT;     // G transpose

    Vec<T> c;
    Vec<T> b;
    Vec<T> h;

    isize n_lb;
    isize n_ub;

    Vec<Eigen::Index> x_lb_idx; // stores the original index of the finite lower bounds
    Vec<Eigen::Index> x_ub_idx; // stores the original index of the finite upper bounds

    Vec<T> x_b_scaling; // scaling of lb and ub, i.e. x_lb <= x_b_scaling .* x <= x_ub

    Vec<T> x_lb_n; // stores negative finite lower bounds in the first n_lb fields
    Vec<T> x_ub;   // stores finite upper bounds in the first n_ub fields

    Data() = default;

    explicit Data(Model<T> model)
    : n(model.P.rows()), p(model.A.rows()), m(model.G.rows()),
      P_utri(model.P.template triangularView<Eigen::Upper>()),
      AT(model.A.transpose()), GT(model.G.transpose()),
      c(model.c), b(model.b), h(model.h),
      n_lb(0), n_ub(0),
      x_lb_idx(model.P.rows()), x_ub_idx(model.P.rows()),
      x_b_scaling(Vec<T>::Constant(model.P.rows(), T(1))),
      x_lb_n(model.P.rows()), x_ub(model.P.rows())
    {
        isize i_lb = 0;
        for (isize i = 0; i < n; i++)
        {
            if (model.x_lb(i) > -PIQP_INF)
            {
                n_lb += 1;
                x_lb_n(i_lb) = -model.x_lb(i);
                x_lb_idx(i_lb) = i;
                i_lb++;
            }
        }

        isize i_ub = 0;
        for (isize i = 0; i < n; i++)
        {
            if (model.x_ub(i) < PIQP_INF)
            {
                n_ub += 1;
                x_ub(i_ub) = model.x_ub(i);
                x_ub_idx(i_ub) = i;
                i_ub++;
            }
        }
    }

    void set_G_row_zero(Eigen::Index row)
    {
        GT.col(row).setZero();
    }

    Eigen::Index non_zeros_P_utri() { return P_utri.rows() * (P_utri.rows() - 1) / 2; }
    Eigen::Index non_zeros_A() { return AT.rows() * AT.cols(); }
    Eigen::Index non_zeros_G() { return GT.rows() * GT.cols(); }
};

} // namespace dense

} // namespace piqp

#ifdef PIQP_WITH_TEMPLATE_INSTANTIATION
#include "piqp/dense/data.tpp"
#endif

#endif //PIQP_DENSE_DATA_HPP
