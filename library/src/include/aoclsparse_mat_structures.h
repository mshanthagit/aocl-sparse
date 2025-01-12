/* ************************************************************************
 * Copyright (c) 2022 Advanced Micro Devices, Inc.
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

#pragma once
#ifndef AOCLSPARSE_MAT_STRUCTS_H
#define AOCLSPARSE_MAT_STRUCTS_H

#include "aoclsparse.h"

/********************************************************************************
 * \brief aoclsparse_csr is a structure holding the aoclsparse matrix
 * in csr format. It must be initialized using aoclsparse_create_(s/d)csr()
 * and the retured handle must be passed to all subsequent library function
 * calls that involve the matrix.
 * It should be destroyed at the end using aoclsparse_destroy_mat_structs().
 *******************************************************************************/
struct _aoclsparse_csr
{
    // CSR matrix part
    aoclsparse_int* csr_row_ptr = nullptr;
    aoclsparse_int* csr_col_ptr = nullptr;
    void*          csr_val     = nullptr;
};



/********************************************************************************
 * \brief aoclsparse_ell is a structure holding the aoclsparse matrix
 * in ELL format. It is used internally during the optimization process.
 * It should be destroyed at the end using aoclsparse_destroy_mat_structs().
 *******************************************************************************/
struct _aoclsparse_ell
{
    // ELL matrix part
    aoclsparse_int ell_width = 0;
    aoclsparse_int* ell_col_ind = nullptr;
    void*          ell_val     = nullptr;
};



/********************************************************************************
 * \brief aoclsparse_ell_csr_hyb is a structure holding the aoclsparse matrix
 * in ELL-CSR hybrid format. It is used internally during the optimization process.
 * It should be destroyed at the end using aoclsparse_destroy_mat_structs().
 *******************************************************************************/
struct _aoclsparse_ell_csr_hyb
{
    // ELL matrix part
    aoclsparse_int ell_width = 0;
    aoclsparse_int ell_m = 0;
    aoclsparse_int* ell_col_ind = nullptr;
    void*          ell_val     = nullptr;

    // CSR part
    aoclsparse_int* csr_row_id_map =  nullptr;
 //   aoclsparse_int* row_id_map =  nullptr;
    aoclsparse_int* csr_col_ptr = nullptr;   // points to the corresponding CSR pointer
    void*          csr_val     = nullptr;    // points to the corresponding CSR pointer


};

/********************************************************************************
 * \brief _aoclsparse_ilu is a structure holding data members for ILU operation.
 * It is used internally during the optimization process which includes ILU factorization.
 * It should be destroyed at the end using aoclsparse_destroy_mat_structs().
 *******************************************************************************/
struct _aoclsparse_ilu
{
    aoclsparse_int         *lu_diag_ptr;        //pointer to diagonal elements in csr values array
    aoclsparse_int         *col_idx_mapper;     //working array
    bool                    ilu_factorized = false;     //flag to indicate if ILU factorization is done
    aoclsparse_ilu_type     ilu_fact_type;              // indicator of ILU factorization type
};
/********************************************************************************
 * \brief _aoclsparse_matrix is a structure holding generic aoclsparse matrices.
 * It should be used by all the sparse routines to initialize the sparse matrices.
 * It should be destroyed at the end using aoclsparse_destroy_mat_structs().
 *******************************************************************************/
struct _aoclsparse_matrix
{
    // generic sparse matrix properties
    aoclsparse_int m;
    aoclsparse_int n;
    aoclsparse_int nnz;
    bool optimized = false;
    aoclsparse_index_base base = aoclsparse_index_base_zero;
    aoclsparse_matrix_data_type val_type = aoclsparse_dmat;
    aoclsparse_matrix_format_type mat_type = aoclsparse_csr_mat;
    aoclsparse_hint_type hint_id = aoclsparse_none;

    // csr matrix
    struct _aoclsparse_csr csr_mat;

    // csr matrix for avx2
    struct _aoclsparse_csr csr_mat_br4;

    // ellt matrix
    struct _aoclsparse_ell ell_mat;

    // ell-csr-hyb matrix
    struct _aoclsparse_ell_csr_hyb ell_csr_hyb_mat;

    //ilu members
    struct _aoclsparse_ilu ilu_info;
};


#endif // AOCLSPARSE_MAT_STRUCTS_H
