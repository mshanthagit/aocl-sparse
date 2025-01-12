/* ************************************************************************
 * Copyright (c) 2022 Advanced Micro Devices, Inc. All rights reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 *
 * ************************************************************************ */

#include "aoclsparse.hpp"

#include <aoclsparse.h>

template <>
aoclsparse_status aoclsparse_csr2m<double>(
        aoclsparse_operation       transA,
        const aoclsparse_mat_descr descrA,
        const aoclsparse_matrix    csrA,
        aoclsparse_operation       transB,
        const aoclsparse_mat_descr descrB,
        const aoclsparse_matrix    csrB,
        aoclsparse_request         request,
        aoclsparse_matrix          *csrC)
{
    return aoclsparse_dcsr2m(
            transA,
            descrA,
            csrA,
            transB,
            descrB,
            csrB,
            request,
            csrC);
}

template <>
aoclsparse_status aoclsparse_csr2m<float>(
        aoclsparse_operation       transA,
        const aoclsparse_mat_descr descrA,
        const aoclsparse_matrix    csrA,
        aoclsparse_operation       transB,
        const aoclsparse_mat_descr descrB,
        const aoclsparse_matrix    csrB,
        aoclsparse_request         request,
        aoclsparse_matrix          *csrC)
{
    return aoclsparse_scsr2m(
            transA,
            descrA,
            csrA,
            transB,
            descrB,
            csrB,
            request,
            csrC);
}

template <>
aoclsparse_status aoclsparse_csrmm(
	aoclsparse_operation       trans,
	const float*               alpha,
	const aoclsparse_matrix    csr,
	const aoclsparse_mat_descr descr,
	aoclsparse_order           order,
	const float*               B,
	aoclsparse_int             n,
	aoclsparse_int             ldb,
	const float*               beta,
	float*                     C,
	aoclsparse_int             ldc)
{
    return aoclsparse_scsrmm(trans,
	    alpha,
	    csr,
	    descr,
	    order,
	    B,
	    n,
	    ldb,
	    beta,
	    C,
	    ldc);
}

template <>
aoclsparse_status aoclsparse_csrmm(
	aoclsparse_operation       trans,
	const double*              alpha,
	const aoclsparse_matrix    csr,
	const aoclsparse_mat_descr descr,
	aoclsparse_order           order,
	const double*              B,
	aoclsparse_int             n,
	aoclsparse_int             ldb,
	const double*              beta,
	double*                    C,
	aoclsparse_int             ldc)
{
    return aoclsparse_dcsrmm(trans,
	    alpha,
	    csr,
	    descr,
	    order,
	    B,
	    n,
	    ldb,
	    beta,
	    C,
	    ldc);
}

template <>
aoclsparse_status aoclsparse_csrmv(
	aoclsparse_operation       trans,
	const float*               alpha,
	aoclsparse_int             m,
	aoclsparse_int             n,
	aoclsparse_int             nnz,
	const float*               csr_val,
	const aoclsparse_int*      csr_col_ind,
	const aoclsparse_int*      csr_row_ptr,
	const aoclsparse_mat_descr descr,
	const float*               x,
	const float*               beta,
	float*                     y)
{
    return aoclsparse_scsrmv(trans,
	    alpha,
	    m,
	    n,
	    nnz,
	    csr_val,
	    csr_col_ind,
	    csr_row_ptr,
	    descr,
	    x,
	    beta,
	    y);
}

template <>
aoclsparse_status aoclsparse_csrmv(
	aoclsparse_operation       trans,
	const double*              alpha,
	aoclsparse_int             m,
	aoclsparse_int             n,
	aoclsparse_int             nnz,
	const double*              csr_val,
	const aoclsparse_int*      csr_col_ind,
	const aoclsparse_int*      csr_row_ptr,
	const aoclsparse_mat_descr descr,
	const double*              x,
	const double*              beta,
	double*                    y)
{
    return aoclsparse_dcsrmv(trans,
	    alpha,
	    m,
	    n,
	    nnz,
	    csr_val,
	    csr_col_ind,
	    csr_row_ptr,
	    descr,
	    x,
	    beta,
	    y);
}

template <>
aoclsparse_status aoclsparse_ellmv(
	aoclsparse_operation       trans,
	const float*               alpha,
	aoclsparse_int             m,
	aoclsparse_int             n,
	aoclsparse_int             nnz,
	const float*               ell_val,
	const aoclsparse_int*      ell_col_ind,
	const aoclsparse_int       ell_width,
	const aoclsparse_mat_descr descr,
	const float*               x,
	const float*               beta,
	float*                     y)
{
    return aoclsparse_sellmv(trans,
	    alpha,
	    m,
	    n,
	    nnz,
	    ell_val,
	    ell_col_ind,
	    ell_width,
	    descr,
	    x,
	    beta,
	    y);

}

template <>
aoclsparse_status aoclsparse_ellmv(
	aoclsparse_operation       trans,
	const double*              alpha,
	aoclsparse_int             m,
	aoclsparse_int             n,
	aoclsparse_int             nnz,
	const double*              ell_val,
	const aoclsparse_int*      ell_col_ind,
	const aoclsparse_int       ell_width,
	const aoclsparse_mat_descr descr,
	const double*              x,
	const double*              beta,
	double*                    y)
{
    return aoclsparse_dellmv(trans,
	    alpha,
	    m,
	    n,
	    nnz,
	    ell_val,
	    ell_col_ind,
	    ell_width,
	    descr,
	    x,
	    beta,
	    y);

}

template <>
aoclsparse_status aoclsparse_elltmv(
        aoclsparse_operation       trans,
        const float*               alpha,
        aoclsparse_int             m,
        aoclsparse_int             n,
        aoclsparse_int             nnz,
        const float*               ell_val,
        const aoclsparse_int*      ell_col_ind,
        const aoclsparse_int       ell_width,
        const aoclsparse_mat_descr descr,
        const float*               x,
        const float*               beta,
        float*                     y)
{
    return aoclsparse_selltmv(trans,
            alpha,
            m,
            n,
            nnz,
            ell_val,
            ell_col_ind,
            ell_width,
            descr,
            x,
            beta,
            y);

}

template <>
aoclsparse_status aoclsparse_elltmv(
        aoclsparse_operation       trans,
        const double*              alpha,
        aoclsparse_int             m,
        aoclsparse_int             n,
        aoclsparse_int             nnz,
        const double*              ell_val,
        const aoclsparse_int*      ell_col_ind,
        const aoclsparse_int       ell_width,
        const aoclsparse_mat_descr descr,
        const double*              x,
        const double*              beta,
        double*                    y)
{
    return aoclsparse_delltmv(trans,
            alpha,
            m,
            n,
            nnz,
            ell_val,
            ell_col_ind,
            ell_width,
            descr,
            x,
            beta,
            y);

}

template <>
aoclsparse_status aoclsparse_ellthybmv(
        aoclsparse_operation       trans,
        const float*               alpha,
        aoclsparse_int             m,
        aoclsparse_int             n,
        aoclsparse_int             nnz,
        const float*               ell_val,
        const aoclsparse_int*      ell_col_ind,
        const aoclsparse_int       ell_width,
        const aoclsparse_int       ell_m,
        const float*               csr_val,
        const aoclsparse_int*      csr_row_ind,
        const aoclsparse_int*      csr_col_ind,
	aoclsparse_int*            row_idx_map,
	aoclsparse_int*            csr_row_idx_map,
        const aoclsparse_mat_descr descr,
        const float*               x,
        const float*               beta,
        float*                     y)
{
    return aoclsparse_sellthybmv(trans,
            alpha,
            m,
            n,
            nnz,
            ell_val,
            ell_col_ind,
            ell_width,
	    ell_m,
	    csr_val,
	    csr_row_ind,
	    csr_col_ind,
	    row_idx_map,
	    csr_row_idx_map,
            descr,
            x,
            beta,
            y);

}

template <>
aoclsparse_status aoclsparse_ellthybmv(
        aoclsparse_operation       trans,
        const double*              alpha,
        aoclsparse_int             m,
        aoclsparse_int             n,
        aoclsparse_int             nnz,
        const double*              ell_val,
        const aoclsparse_int*      ell_col_ind,
        const aoclsparse_int       ell_width,
        const aoclsparse_int       ell_m,
        const double*               csr_val,
        const aoclsparse_int*      csr_row_ind,
        const aoclsparse_int*      csr_col_ind,
        aoclsparse_int*            row_idx_map,
        aoclsparse_int*            csr_row_idx_map,
        const aoclsparse_mat_descr descr,
        const double*              x,
        const double*              beta,
        double*                    y)
{
    return aoclsparse_dellthybmv(trans,
            alpha,
            m,
            n,
            nnz,
            ell_val,
            ell_col_ind,
            ell_width,
	    ell_m,
            csr_val,
            csr_row_ind,
            csr_col_ind,
            row_idx_map,
            csr_row_idx_map,
            descr,
            x,
            beta,
            y);

}

template <>
aoclsparse_status aoclsparse_mv(
		aoclsparse_operation       op,
        const float*               alpha,
        aoclsparse_matrix       A,
        const aoclsparse_mat_descr descr,
        const float*               x,
        const float*               beta,
        float*                     y)
{
    return aoclsparse_smv(op,
            alpha,
	        A,
            descr,
            x,
            beta,
            y);

}

template <>
aoclsparse_status aoclsparse_mv(
		aoclsparse_operation       op,
        const double*               alpha,
        aoclsparse_matrix       A,
        const aoclsparse_mat_descr descr,
        const double*               x,
        const double*               beta,
        double*                     y)
{
    return aoclsparse_dmv(op,
            alpha,
			A,
            descr,
            x,
            beta,
            y);

}

template <>
aoclsparse_status aoclsparse_diamv(
	aoclsparse_operation       trans,
	const float*               alpha,
	aoclsparse_int             m,
	aoclsparse_int             n,
	aoclsparse_int             nnz,
	const float*               dia_val,
	const aoclsparse_int*      dia_offset,
	aoclsparse_int             dia_num_diag,
	const aoclsparse_mat_descr descr,
	const float*               x,
	const float*               beta,
	float*                     y )
{
    return aoclsparse_sdiamv(trans,
	    alpha,
	    m,
	    n,
	    nnz,
	    dia_val,
	    dia_offset,
	    dia_num_diag,
	    descr,
	    x,
	    beta,
	    y);
}

template <>
aoclsparse_status aoclsparse_diamv(
	aoclsparse_operation       trans,
	const double*              alpha,
	aoclsparse_int             m,
	aoclsparse_int             n,
	aoclsparse_int             nnz,
	const double*              dia_val,
	const aoclsparse_int*      dia_offset,
	aoclsparse_int             dia_num_diag,
	const aoclsparse_mat_descr descr,
	const double*              x,
	const double*              beta,
	double*                    y )
{
    return aoclsparse_ddiamv(trans,
	    alpha,
	    m,
	    n,
	    nnz,
	    dia_val,
	    dia_offset,
	    dia_num_diag,
	    descr,
	    x,
	    beta,
	    y);

}

template <>
aoclsparse_status aoclsparse_bsrmv(
	aoclsparse_operation       trans,
	const float*               alpha,
	aoclsparse_int             mb,
	aoclsparse_int             nb,
	aoclsparse_int             bsr_dim,
	const float*               bsr_val,
	const aoclsparse_int*      bsr_col_ind,
	const aoclsparse_int*      bsr_row_ptr,
	const aoclsparse_mat_descr descr,
	const float*               x,
	const float*               beta,
	float*                     y)
{
    return aoclsparse_sbsrmv(trans,
	    alpha,
	    mb,
	    nb,
	    bsr_dim,
	    bsr_val,
	    bsr_col_ind,
	    bsr_row_ptr,
	    descr,
	    x,
	    beta,
	    y);
}

template <>
aoclsparse_status aoclsparse_bsrmv(
	aoclsparse_operation       trans,
	const double*              alpha,
	aoclsparse_int             mb,
	aoclsparse_int             nb,
	aoclsparse_int             bsr_dim,
	const double*              bsr_val,
	const aoclsparse_int*      bsr_col_ind,
	const aoclsparse_int*      bsr_row_ptr,
	const aoclsparse_mat_descr descr,
	const double*              x,
	const double*              beta,
	double*                    y)
{
    return aoclsparse_dbsrmv(trans,
	    alpha,
	    mb,
	    nb,
	    bsr_dim,
	    bsr_val,
	    bsr_col_ind,
	    bsr_row_ptr,
	    descr,
	    x,
	    beta,
	    y);

}

template <>
aoclsparse_status aoclsparse_csrsv(
	aoclsparse_operation       trans,
	const float*               alpha,
	aoclsparse_int             m,
	const float*               csr_val,
	const aoclsparse_int*      csr_col_ind,
	const aoclsparse_int*      csr_row_ptr,
	const aoclsparse_mat_descr descr,
	const float*               x,
	float*                     y )
{
    return aoclsparse_scsrsv(trans,
	    alpha,
	    m,
	    csr_val,
	    csr_col_ind,
	    csr_row_ptr,
	    descr,
	    x,
	    y);
}

template <>
aoclsparse_status aoclsparse_csrsv(
	aoclsparse_operation       trans,
	const double*              alpha,
	aoclsparse_int             m,
	const double*              csr_val,
	const aoclsparse_int*      csr_col_ind,
	const aoclsparse_int*      csr_row_ptr,
	const aoclsparse_mat_descr descr,
	const double*              x,
	double*                    y )
{
    return aoclsparse_dcsrsv(trans,
	    alpha,
	    m,
	    csr_val,
	    csr_col_ind,
	    csr_row_ptr,
	    descr,
	    x,
	    y);
}

template <>
aoclsparse_status aoclsparse_csr2ell(
	aoclsparse_int       m,
	const aoclsparse_int *csr_row_ptr,
	const aoclsparse_int *csr_col_ind,
	const float          *csr_val,
	aoclsparse_int       *ell_col_ind,
	float                *ell_val,
	aoclsparse_int       ell_width)
{
    return aoclsparse_scsr2ell(m,
	    csr_row_ptr,
	    csr_col_ind,
	    csr_val,
	    ell_col_ind,
	    ell_val,
	    ell_width);
}

template <>
aoclsparse_status aoclsparse_csr2ell(
	aoclsparse_int       m,
	const aoclsparse_int *csr_row_ptr,
	const aoclsparse_int *csr_col_ind,
	const double         *csr_val,
	aoclsparse_int       *ell_col_ind,
	double               *ell_val,
	aoclsparse_int       ell_width)
{
    return aoclsparse_dcsr2ell(m,
	    csr_row_ptr,
	    csr_col_ind,
	    csr_val,
	    ell_col_ind,
	    ell_val,
	    ell_width);
}

template <>
aoclsparse_status aoclsparse_csr2ellt(
        aoclsparse_int       m,
        const aoclsparse_int *csr_row_ptr,
        const aoclsparse_int *csr_col_ind,
        const float          *csr_val,
        aoclsparse_int       *ell_col_ind,
        float                *ell_val,
        aoclsparse_int       ell_width)
{
    return aoclsparse_scsr2ellt(m,
            csr_row_ptr,
            csr_col_ind,
            csr_val,
            ell_col_ind,
            ell_val,
            ell_width);
}

template <>
aoclsparse_status aoclsparse_csr2ellt(
        aoclsparse_int       m,
        const aoclsparse_int *csr_row_ptr,
        const aoclsparse_int *csr_col_ind,
        const double         *csr_val,
        aoclsparse_int       *ell_col_ind,
        double               *ell_val,
        aoclsparse_int       ell_width)
{
    return aoclsparse_dcsr2ellt(m,
            csr_row_ptr,
            csr_col_ind,
            csr_val,
            ell_col_ind,
            ell_val,
            ell_width);
}


template <>
aoclsparse_status aoclsparse_csr2ellthyb(
        aoclsparse_int       m,
        aoclsparse_int       *ell_m,
        const aoclsparse_int *csr_row_ptr,
        const aoclsparse_int *csr_col_ind,
        const float          *csr_val,
        aoclsparse_int       *row_idx_map,
        aoclsparse_int       *csr_row_idx_map,
        aoclsparse_int       *ell_col_ind,
        float                *ell_val,
        aoclsparse_int       ell_width)
{
    return aoclsparse_scsr2ellthyb(m,
            ell_m,
            csr_row_ptr,
            csr_col_ind,
            csr_val,
	    row_idx_map,
	    csr_row_idx_map,
            ell_col_ind,
            ell_val,
            ell_width);
}

template <>
aoclsparse_status aoclsparse_csr2ellthyb(
        aoclsparse_int       m,
        aoclsparse_int       *ell_m,
        const aoclsparse_int *csr_row_ptr,
        const aoclsparse_int *csr_col_ind,
        const double         *csr_val,
        aoclsparse_int       *row_idx_map,
        aoclsparse_int       *csr_row_idx_map,
        aoclsparse_int       *ell_col_ind,
        double               *ell_val,
        aoclsparse_int       ell_width)
{
    return aoclsparse_dcsr2ellthyb(m,
	    ell_m,
            csr_row_ptr,
            csr_col_ind,
            csr_val,
            row_idx_map,
            csr_row_idx_map,
            ell_col_ind,
            ell_val,
            ell_width);
}

template <>
aoclsparse_status aoclsparse_csr2dia(
        aoclsparse_int       m,
        aoclsparse_int       n,
        const aoclsparse_int *csr_row_ptr,
        const aoclsparse_int *csr_col_ind,
        const float          *csr_val,
        aoclsparse_int       dia_num_diag,
        aoclsparse_int       *dia_offset,
        float                *dia_val)
{
    return aoclsparse_scsr2dia(m,
	    n,
	    csr_row_ptr,
	    csr_col_ind,
	    csr_val,
	    dia_num_diag,
	    dia_offset,
	    dia_val);
}

template <>
aoclsparse_status aoclsparse_csr2dia(
        aoclsparse_int       m,
        aoclsparse_int       n,
        const aoclsparse_int *csr_row_ptr,
        const aoclsparse_int *csr_col_ind,
        const double         *csr_val,
        aoclsparse_int       dia_num_diag,
        aoclsparse_int       *dia_offset,
        double               *dia_val)
{
    return aoclsparse_dcsr2dia(m,
	    n,
	    csr_row_ptr,
	    csr_col_ind,
	    csr_val,
	    dia_num_diag,
	    dia_offset,
	    dia_val);
}

template <>
aoclsparse_status aoclsparse_csr2bsr(
	aoclsparse_int       m,
	aoclsparse_int       n,
	const float          *csr_val,
	const aoclsparse_int *csr_row_ptr,
	const aoclsparse_int *csr_col_ind,
	aoclsparse_int       block_dim,
	float                *bsr_val,
	aoclsparse_int       *bsr_row_ptr,
	aoclsparse_int       *bsr_col_ind)
{
    return aoclsparse_scsr2bsr(m,
	    n,
	    csr_val,
	    csr_row_ptr,
	    csr_col_ind,
	    block_dim,
	    bsr_val,
	    bsr_row_ptr,
	    bsr_col_ind);
}

template <>
aoclsparse_status aoclsparse_csr2bsr(
	aoclsparse_int       m,
	aoclsparse_int       n,
	const double         *csr_val,
	const aoclsparse_int *csr_row_ptr,
	const aoclsparse_int *csr_col_ind,
	aoclsparse_int       block_dim,
	double               *bsr_val,
	aoclsparse_int       *bsr_row_ptr,
	aoclsparse_int       *bsr_col_ind)
{
    return aoclsparse_dcsr2bsr(m,
	    n,
	    csr_val,
	    csr_row_ptr,
	    csr_col_ind,
	    block_dim,
	    bsr_val,
	    bsr_row_ptr,
	    bsr_col_ind);
}

template <>
aoclsparse_status aoclsparse_csr2csc(
        aoclsparse_int       m,
        aoclsparse_int       n,
        aoclsparse_int       nnz,
        const aoclsparse_int *csr_row_ptr,
        const aoclsparse_int *csr_col_ind,
        const float          *csr_val,
        aoclsparse_int       *csc_row_ind,
        aoclsparse_int       *csc_col_ptr,
        float                *csc_val)
{
    return aoclsparse_scsr2csc(m,
	    n,
	    nnz,
	    csr_row_ptr,
	    csr_col_ind,
	    csr_val,
	    csc_row_ind,
	    csc_col_ptr,
	    csc_val);
}

template <>
aoclsparse_status aoclsparse_csr2csc(
        aoclsparse_int       m,
        aoclsparse_int       n,
        aoclsparse_int       nnz,
        const aoclsparse_int *csr_row_ptr,
        const aoclsparse_int *csr_col_ind,
        const double         *csr_val,
        aoclsparse_int       *csc_row_ind,
        aoclsparse_int       *csc_col_ptr,
        double               *csc_val)
{
    return aoclsparse_dcsr2csc(m,
	    n,
	    nnz,
	    csr_row_ptr,
	    csr_col_ind,
	    csr_val,
	    csc_row_ind,
	    csc_col_ptr,
	    csc_val);
}

template <>
aoclsparse_status aoclsparse_csr2dense(
            aoclsparse_int             m,
            aoclsparse_int             n,
            const aoclsparse_mat_descr descr,
            const float*               csr_val,
            const aoclsparse_int*      csr_row_ptr,
            const aoclsparse_int*      csr_col_ind,
            float*                     A,
            aoclsparse_int             ld,
            aoclsparse_order           order)
{
    return aoclsparse_scsr2dense(m,
            n,
	    descr,
	    csr_val,
	    csr_row_ptr,
	    csr_col_ind,
	    A,
	    ld,
            order);
}


template <>
aoclsparse_status aoclsparse_csr2dense(
            aoclsparse_int             m,
            aoclsparse_int             n,
            const aoclsparse_mat_descr descr,
            const double*              csr_val,
            const aoclsparse_int*      csr_row_ptr,
            const aoclsparse_int*      csr_col_ind,
            double*                    A,
            aoclsparse_int             ld,
            aoclsparse_order           order)
{
    return aoclsparse_dcsr2dense(m,
            n,
	    descr,
	    csr_val,
	    csr_row_ptr,
	    csr_col_ind,
	    A,
	    ld,
	    order);
}

template <>
aoclsparse_status aoclsparse_ilu_smoother(
	aoclsparse_operation       	op,
	aoclsparse_matrix       	A,
	const aoclsparse_mat_descr 	descr,
	const float*                diag,
	const float*                approx_inv_diag,
	float*               		x,
	const float*                b)
{
    return aoclsparse_silu_smoother(op,
	    A,
	    descr,
	    diag,
	    approx_inv_diag,
	    x,
	    b);

}

template <>
aoclsparse_status aoclsparse_ilu_smoother(
	aoclsparse_operation       			op,
	aoclsparse_matrix       			A,
	const aoclsparse_mat_descr 			descr,
	const double*                   	diag,
	const double*                   	approx_inv_diag,
	double*               				x,
	const double*                     	b)
{
    return aoclsparse_dilu_smoother(op,
	    A,
	    descr,
	    diag,
	    approx_inv_diag,
	    x,
	    b);

}

template <>
aoclsparse_status aoclsparse_create_csr(aoclsparse_matrix &mat,
                    aoclsparse_index_base   base,
                    aoclsparse_int          M,
                    aoclsparse_int          N,
                    aoclsparse_int          csr_nnz,
                    aoclsparse_int*         csr_row_ptr,
                    aoclsparse_int*         csr_col_ptr,
                    float*                  csr_val)
{
    return aoclsparse_create_scsr(mat,
	    base,
	    M,
	    N,
	    csr_nnz,
	    csr_row_ptr,
	    csr_col_ptr,
	    csr_val);
}
template <>
aoclsparse_status aoclsparse_create_csr(aoclsparse_matrix &mat,
                    aoclsparse_index_base   base,
                    aoclsparse_int          M,
                    aoclsparse_int          N,
                    aoclsparse_int          csr_nnz,
                    aoclsparse_int*         csr_row_ptr,
                    aoclsparse_int*         csr_col_ptr,
                    double*                 csr_val)
{
    return aoclsparse_create_dcsr(mat,
	    base,
	    M,
	    N,
	    csr_nnz,
	    csr_row_ptr,
	    csr_col_ptr,
	    csr_val);
}
