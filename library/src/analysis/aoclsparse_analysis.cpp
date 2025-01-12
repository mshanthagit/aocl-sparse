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

#include "aoclsparse.h"
#include "aoclsparse_analysis.hpp"
#include <algorithm>
#include <cmath>
#include <limits>
/*
 *===========================================================================
 *   C wrapper
 * ===========================================================================
 */
aoclsparse_status aoclsparse_optimize_mv(aoclsparse_matrix A)
{
    // Check the matrix precision type
    // If single, set the matrix type as aoclsparse_csr_mat
    // and return withtout any optimization

    if (A->val_type == aoclsparse_smat) {
         A->mat_type = aoclsparse_csr_mat;
         A->optimized = true;
         return aoclsparse_status_success;
    }

    
    // collect the required for decision making
    aoclsparse_int *row_ptr = A->csr_mat.csr_row_ptr;
    aoclsparse_int m = A->m;
    // 1: ELL width
    aoclsparse_int ell_width = 0, nnz = A->nnz;
    double nnza = (double)nnz/m;
    aoclsparse_int mx_nnz_lt_nnza = 0, mn_nnz_gt_nnza = nnz, cmn = 0, cmx = 0;
    for(aoclsparse_int i = 0; i < m; ++i)
    {
        aoclsparse_int nnzi = row_ptr[i + 1] - row_ptr[i];
        if ((nnzi > mx_nnz_lt_nnza) && (nnzi <= nnza)) {
            mx_nnz_lt_nnza = nnzi;
        }
        if ((nnzi < mn_nnz_gt_nnza) && (nnzi > nnza)) {
            mn_nnz_gt_nnza = nnzi;
        }

        if ((nnzi <= nnza))
            cmx++;
        else
            cmn++;
    }
    if (cmx >= cmn)
       ell_width = mx_nnz_lt_nnza;
    else
       ell_width = mn_nnz_gt_nnza;

    // 2: csr_rows_with_nnz_lt_10, ell_csr_nnz (hybrid fillin), ...
    aoclsparse_int ell_m = 0, ell_csr_nnz = 0, ell_csr_g_ew_l_10 = 0,  ell_csr_g_ew_g_10 = 0, csr_lt_10 = 0, rem = 0;
    for(aoclsparse_int i = 0; i < m; ++i)
    {
        aoclsparse_int row_nnz = row_ptr[i + 1] - row_ptr[i];
        if (row_nnz <= ell_width)
           (ell_m)++;
        else {
            ell_csr_nnz += row_nnz;
            if (row_nnz <= 10)
                    ell_csr_g_ew_l_10++;
            else
                    ell_csr_g_ew_g_10++;
        }
        if (row_nnz <= 10)
                csr_lt_10++;
        rem += row_nnz%4;
    }

    aoclsparse_int ell_nnz = ell_width*m + ell_csr_nnz;

    // 3: Fill-in for csr_br4 implementation
    aoclsparse_int i,j, tnnz = 0;
    aoclsparse_int row_nnz;
    for (i = 0 ; i < m; i+=4){

        aoclsparse_int m1, m2;
        if ((m - i) < 4)
            break;
        m1 = std::max((row_ptr[i+1]-row_ptr[i]),
                        (row_ptr[i+2]-row_ptr[i+1]) );
        m2 = std::max((row_ptr[i+3]-row_ptr[i+2]),
                        (row_ptr[i+4]-row_ptr[i+3]) );
        row_nnz = std::max(m1,m2);
        tnnz += 4*row_nnz;
    }
    for (j = i; j < m; ++j) {
        row_nnz = row_ptr[j+1]-row_ptr[j];
        tnnz += row_nnz;
    }
    double prctg_rows_lt_10 = (double)(csr_lt_10/m)*100;
    double fill_ratio = ((double)(tnnz-ell_nnz)/tnnz)*100;

    if (ell_width <= 30){ // ToDo: why this cutoff?
      if (fill_ratio < -0.1) {
          A->mat_type = aoclsparse_csr_mat_br4;   // CSR-BT-AVX2
      } else if ((fill_ratio > -0.1) && (fill_ratio < 0.1)){
          A->mat_type = aoclsparse_ellt_csr_hyb_mat;   // ELL-CSR-HYB 
      } else {
          if ( ell_csr_g_ew_l_10 < ell_csr_g_ew_g_10) {
              A->mat_type = aoclsparse_ellt_csr_hyb_mat; // ELL-CSR-HYB 
          } else {
             A->mat_type = aoclsparse_csr_mat_br4;  // CSR-BT-AVX2
          }
      }
    } else {
      if (prctg_rows_lt_10 < 10.0) {
         A->mat_type = aoclsparse_csr_mat;     // CSR 
      } else {
         A->mat_type = aoclsparse_csr_mat_br4; // CSR-BT-AVX2
      }
    }

    if (A->mat_type == aoclsparse_ellt_csr_hyb_mat) {

        aoclsparse_int *ell_col_ind; 
        double *ell_dval;
        float *ell_sval;
        aoclsparse_int *csr_row_idx_map;
        aoclsparse_int *row_idx_map;
        aoclsparse_int              ell_width;
        aoclsparse_int              ell_m;

        // get the ell_width
        aoclsparse_csr2ellthyb_width(A->m, A->nnz, A->csr_mat.csr_row_ptr, &ell_m, &ell_width);

        ell_col_ind = (aoclsparse_int *) malloc(sizeof(aoclsparse_int)*ell_width * A->m);

        if (A->val_type == aoclsparse_dmat) {
            ell_dval = (double *) malloc(sizeof(double)*ell_width * A->m);
        } else if (A->val_type == aoclsparse_smat) {
            ell_sval = (float *) malloc(sizeof(float)*ell_width * A->m);
        }
        csr_row_idx_map = (aoclsparse_int*) malloc(sizeof(aoclsparse_int)*(A->m - ell_m));

        // convert to hybrid ELLT-CSR format
        if (A->val_type == aoclsparse_dmat) {
            aoclsparse_dcsr2ellthyb(A->m, &ell_m, A->csr_mat.csr_row_ptr, A->csr_mat.csr_col_ptr, 
            (double *)A->csr_mat.csr_val, row_idx_map, csr_row_idx_map, ell_col_ind, ell_dval, ell_width);
            A->ell_csr_hyb_mat.ell_val = (double*)ell_dval;
        } else if (A->val_type == aoclsparse_smat) {
            aoclsparse_scsr2ellthyb(A->m, &ell_m, A->csr_mat.csr_row_ptr, A->csr_mat.csr_col_ptr, 
            (float *) A->csr_mat.csr_val, row_idx_map, csr_row_idx_map, ell_col_ind, ell_sval, ell_width);
            A->ell_csr_hyb_mat.ell_val = (float*)ell_sval;
        }

        // set appropriate members of "A"
        A->ell_csr_hyb_mat.ell_width = ell_width;
        A->ell_csr_hyb_mat.ell_m = ell_m;
        A->ell_csr_hyb_mat.ell_col_ind = ell_col_ind;
        A->ell_csr_hyb_mat.csr_row_id_map = csr_row_idx_map;
        A->ell_csr_hyb_mat.csr_col_ptr = A->csr_mat.csr_col_ptr;
        A->ell_csr_hyb_mat.csr_val = A->csr_mat.csr_val;

    } else if (A->mat_type == aoclsparse_csr_mat_br4) {  // vectorized csr blocked format for AVX2
        aoclsparse_int *col_ptr;
        aoclsparse_int *row_ptr;
        aoclsparse_int *trow_ptr;   // ToDo: need to replace row_ptr with trow_ptr
        void *csr_val;
        aoclsparse_int row_nnz;

        // populate row_nnz
        aoclsparse_int i;
        aoclsparse_int j;
        aoclsparse_int tnnz = 0;
        row_ptr =(aoclsparse_int *) malloc(sizeof(aoclsparse_int) * (A->m));
        trow_ptr =(aoclsparse_int *) malloc(sizeof(aoclsparse_int) * (A->m + 1));
        trow_ptr[0] = A->base;
        for (i = 0 ; i < A->m; i+=4){

            aoclsparse_int m1, m2;
            if ((A->m - i) < 4)
               break;
            m1 = std::max((A->csr_mat.csr_row_ptr[i+1]-A->csr_mat.csr_row_ptr[i]),
                           (A->csr_mat.csr_row_ptr[i+2]-A->csr_mat.csr_row_ptr[i+1]) );
            m2 = std::max((A->csr_mat.csr_row_ptr[i+3]-A->csr_mat.csr_row_ptr[i+2]),
                           (A->csr_mat.csr_row_ptr[i+4]-A->csr_mat.csr_row_ptr[i+3]) );
            row_nnz = std::max(m1,m2);
            row_ptr[i] = row_ptr[i+1] = row_ptr[i+2] = row_ptr[i+3] = row_nnz;
            trow_ptr[i+1] = trow_ptr[i] + row_nnz;
            trow_ptr[i+2] = trow_ptr[i+1] + row_nnz;
            trow_ptr[i+3] = trow_ptr[i+2] + row_nnz;
            trow_ptr[i+4] = trow_ptr[i+3] + row_nnz;
            tnnz += 4*row_nnz;
        }
        for (j = i; j < A->m; ++j) {
            row_nnz = A->csr_mat.csr_row_ptr[j+1]-A->csr_mat.csr_row_ptr[j];
            tnnz += row_nnz;
            row_ptr[j] = row_nnz;
            trow_ptr[j+1] = trow_ptr[j] + row_nnz;
        }

        // create the new csr matrix and convert to the csr-avx2 format
        col_ptr =(aoclsparse_int *) malloc(sizeof(aoclsparse_int) * tnnz);
        if (A->val_type == aoclsparse_dmat) {
            csr_val = (double *) malloc(sizeof(double)*tnnz);
        } else if (A->val_type == aoclsparse_smat) {
            csr_val = (float *) malloc(sizeof(float)*tnnz);
        }
        aoclsparse_int tc = 0; // count of nonzeros
        for (i = 0; i < A->m; ++i) {
            aoclsparse_int nz = A->csr_mat.csr_row_ptr[i+1]-A->csr_mat.csr_row_ptr[i];
            aoclsparse_int ridx = A->csr_mat.csr_row_ptr[i];
            for (j = 0; j < nz; ++j) {
                ((double *)csr_val)[tc] = ((double *)A->csr_mat.csr_val)[ridx+j];
                col_ptr[tc] = A->csr_mat.csr_col_ptr[ridx+j];
                tc++;
            }
            if (nz < row_ptr[i]) { // ToDo -- can remove the if condition
                for (j = nz; j < row_ptr[i]; ++j) {
                    col_ptr[tc] = A->csr_mat.csr_col_ptr[ridx+nz-1];
                    ((double *)csr_val)[tc] = static_cast<double>(0);
                    tc++;
                }
            }
        }
        tc = 0;
        aoclsparse_int *cptr = (aoclsparse_int *) col_ptr;
        double *vptr = (double *) csr_val;
        for(i = 0; i < A->m; i+=4) {
            cptr = col_ptr + tc;
            vptr = (double*)csr_val + tc;
            aoclsparse_int nnz = row_ptr[i];
            if ((A->m - i) < 4)
               break;
            // transponse the chunk into an auxiliary buffer
            double *bufval = (double *)malloc(sizeof(double)*nnz*4);
            aoclsparse_int *bufidx = (aoclsparse_int *)malloc(sizeof(aoclsparse_int)*nnz*4);
            aoclsparse_int ii, jj;
            for (ii = 0; ii < nnz; ++ii) {
                for (jj = 0; jj < 4;  ++jj) {
                    bufval[jj+ii*4] = vptr[ii + jj*nnz];
                    bufidx[jj+ii*4] = cptr[ii + jj*nnz];
                }
            }
            memcpy(vptr, bufval, sizeof(double)*nnz*4);
            memcpy(cptr, bufidx, sizeof(aoclsparse_int)*nnz*4);
            free (bufval);
            free(bufidx);
            tc += nnz*4;
        }

        // set appropriate members of "A"
//        A->csr_mat_br4.csr_row_ptr = row_ptr;
        free(row_ptr);
        A->csr_mat_br4.csr_row_ptr = trow_ptr;   // ToDo: replace row_ptr with this
        A->csr_mat_br4.csr_col_ptr = col_ptr;
        A->csr_mat_br4.csr_val = csr_val;
    }
    
    A->optimized = true;

    return aoclsparse_status_success;
}

aoclsparse_status aoclsparse_optimize_ilu0(aoclsparse_matrix A)
{
    aoclsparse_status       ret = aoclsparse_status_success;
    aoclsparse_int         *lu_diag_ptr = NULL; 
    aoclsparse_int         *col_idx_mapper = NULL; 
    aoclsparse_int          nrows = A->m;

    lu_diag_ptr = (aoclsparse_int *) malloc(sizeof(aoclsparse_int)* nrows);
    if(NULL == lu_diag_ptr)
    {
        ret = aoclsparse_status_internal_error;
        return ret;
    }

    col_idx_mapper = (aoclsparse_int *) malloc(sizeof(aoclsparse_int)* nrows);
    if(NULL == col_idx_mapper)
    {
        ret = aoclsparse_status_internal_error;
        return ret;
    }

    for(aoclsparse_int i = 0; i < nrows; i++)
    {
        col_idx_mapper[i] = 0;
        lu_diag_ptr[i] = 0;
    } 

    //set members of ILU info
    A->ilu_info.lu_diag_ptr = lu_diag_ptr;
    A->ilu_info.col_idx_mapper = col_idx_mapper;
    A->ilu_info.ilu_factorized = false;

    A->optimized = true;
    return ret;
}

aoclsparse_status aoclsparse_optimize_ilu(aoclsparse_matrix A)
{
    aoclsparse_status       ret = aoclsparse_status_success;

    A->ilu_info.ilu_fact_type = aoclsparse_ilu0; // ILU0 
    switch(A->ilu_info.ilu_fact_type)
    {
        case aoclsparse_ilu0:
            ret = aoclsparse_optimize_ilu0(A); 			
            break;    
        case aoclsparse_ilup:            
            //ret = aoclsparse_optimize_ilup(A); 			
            //To Do
            break;                                       
        default:
            ret = aoclsparse_status_invalid_value;
            break;
    }  
    return ret;

}
aoclsparse_status aoclsparse_optimize(aoclsparse_matrix A)
{
    aoclsparse_status       ret = aoclsparse_status_success;
    // Validations

    // check if already optimized
    if (A->optimized) 
    {
        return aoclsparse_status_success;
    } 

    // Check sizes
    if((A->m < 0) || (A->n < 0) ||  (A->nnz < 0))
    {
        return aoclsparse_status_invalid_size;
    }

    // Check CSR matrix is populated, it not return an error. ToDo: need to handle CSC / COO cases later
    if((A->csr_mat.csr_row_ptr == nullptr) || (A->csr_mat.csr_col_ptr == nullptr) || (A->csr_mat.csr_val == nullptr))
    {
        return aoclsparse_status_invalid_pointer;
    }

    // ToDo: any other validations

    aoclsparse_int mv_hint, trsv_hint, mm_hint, twom_hint, ilu_hint;     
    mv_hint = A->hint_id & aoclsparse_spmv;
    trsv_hint = (A->hint_id & aoclsparse_trsv) >> 1;
    mm_hint = (A->hint_id & aoclsparse_mm) >> 2;
    twom_hint = (A->hint_id & aoclsparse_2m) >> 3;
    ilu_hint = (A->hint_id & aoclsparse_ilu) >> 4;

    if(mv_hint)
    {
        //SPMV Analysis and Optimization
        ret = aoclsparse_optimize_mv(A);
    }
    if(trsv_hint)
    {
        //TRSV Analysis and Optimization
        //To Do 
    }    
    if(mm_hint)
    {
        //Dense - Sparse Matrix Mult Analysis and Optimization
        //To Do        
    }
    if(twom_hint)
    {
        //Sparse - Sparse Matrix Mult Analysis and Optimization
        //To Do        
    }
    if(ilu_hint)
    {
        //ILU Analysis and Optimization
        ret = aoclsparse_optimize_ilu(A);        
    }        
    
    return ret;
}

aoclsparse_status aoclsparse_set_mv_hint(aoclsparse_matrix A,
                                                aoclsparse_operation       trans,
                                                const aoclsparse_mat_descr descr,
                                                aoclsparse_int       expected_no_of_calls)
{
    //check descriptor
    if(descr == nullptr)
    {
        return aoclsparse_status_invalid_pointer;
    }    
    // Check index base
    if(descr->base != aoclsparse_index_base_zero)
    {
        // TODO
        return aoclsparse_status_not_implemented;
    }
    if(trans != aoclsparse_operation_none)
    {
        // TODO
        return aoclsparse_status_not_implemented;
    }    
    // Check sizes
    if(A->m < 0)
    {
        return aoclsparse_status_invalid_size;
    }
    else if(A->n < 0)
    {
        return aoclsparse_status_invalid_size;
    }    
    // Sanity check
    if((A->m == 0 || A->n == 0))
    {
        return aoclsparse_status_invalid_size;
    }    
    if(A->nnz < 0)
    {
        return aoclsparse_status_invalid_size;
    }

    // Check CSR matrix is populated, it not return an error. ToDo: need to handle CSC / COO cases later
    if((A->csr_mat.csr_row_ptr == nullptr) || (A->csr_mat.csr_col_ptr == nullptr) || (A->csr_mat.csr_val == nullptr))
    {
        return aoclsparse_status_invalid_pointer;
    }
    aoclsparse_int mv_hint;     
    mv_hint = A->hint_id & aoclsparse_spmv;

    if(!mv_hint)
    {
        A->hint_id = static_cast<aoclsparse_hint_type>(A->hint_id | aoclsparse_spmv);
    }    

    //todo: Any more analysis operations that will help optimize routine later
    //todo: to make use of trans, expected_no_of_calls and matrix descriptor 

    return aoclsparse_status_success;
}
aoclsparse_status aoclsparse_set_sv_hint(aoclsparse_matrix A,
                                                aoclsparse_operation       trans,
                                                const aoclsparse_mat_descr descr,
                                                aoclsparse_int       expected_no_of_calls)
{
    //check descriptor
    if(descr == nullptr)
    {
        return aoclsparse_status_invalid_pointer;
    }    
    // Check index base
    if(descr->base != aoclsparse_index_base_zero)
    {
        // TODO
        return aoclsparse_status_not_implemented;
    }
    if(descr->type != aoclsparse_matrix_type_symmetric)
    {
        // TODO
        return aoclsparse_status_not_implemented;
    }    
    if(trans != aoclsparse_operation_none)
    {
        // TODO
        return aoclsparse_status_not_implemented;
    }    
    // Check sizes
    if(A->m < 0)
    {
        return aoclsparse_status_invalid_size;
    }
    else if(A->n < 0)
    {
        return aoclsparse_status_invalid_size;
    }    
    // Sanity check
    if((A->m == 0 || A->n == 0))
    {
        return aoclsparse_status_invalid_size;
    }    
    if(A->nnz < 0)
    {
        return aoclsparse_status_invalid_size;
    }

    // Check CSR matrix is populated, it not return an error. ToDo: need to handle CSC / COO cases later
    if((A->csr_mat.csr_row_ptr == nullptr) || (A->csr_mat.csr_col_ptr == nullptr) || (A->csr_mat.csr_val == nullptr))
    {
        return aoclsparse_status_invalid_pointer;
    }

    aoclsparse_int trsv_hint;     
    trsv_hint = (A->hint_id & aoclsparse_trsv) >> 1;

    //set if not enabled
    if(!trsv_hint)
    {
        A->hint_id = static_cast<aoclsparse_hint_type>(A->hint_id | aoclsparse_trsv);
    }  

    //todo: Any more analysis operations that will help optimize routine later
    //todo: to make use of trans, expected_no_of_calls and matrix descriptor 

    return aoclsparse_status_success;
}
aoclsparse_status aoclsparse_set_mm_hint(aoclsparse_matrix A,
                                                aoclsparse_operation       trans,
                                                const aoclsparse_mat_descr descr,
                                                aoclsparse_int       expected_no_of_calls)
{
    //check descriptor
    if(descr == nullptr)
    {
        return aoclsparse_status_invalid_pointer;
    }    
    // Check index base
    if(descr->base != aoclsparse_index_base_zero)
    {
        // TODO
        return aoclsparse_status_not_implemented;
    }
    if(descr->type != aoclsparse_matrix_type_symmetric)
    {
        // TODO
        return aoclsparse_status_not_implemented;
    }    
    if(trans != aoclsparse_operation_none)
    {
        // TODO
        return aoclsparse_status_not_implemented;
    }    
    // Check sizes
    if(A->m < 0)
    {
        return aoclsparse_status_invalid_size;
    }
    else if(A->n < 0)
    {
        return aoclsparse_status_invalid_size;
    }    
    // Sanity check
    if((A->m == 0 || A->n == 0))
    {
        return aoclsparse_status_invalid_size;
    }    
    if(A->nnz < 0)
    {
        return aoclsparse_status_invalid_size;
    }

    // Check CSR matrix is populated, it not return an error. ToDo: need to handle CSC / COO cases later
    if((A->csr_mat.csr_row_ptr == nullptr) || (A->csr_mat.csr_col_ptr == nullptr) || (A->csr_mat.csr_val == nullptr))
    {
        return aoclsparse_status_invalid_pointer;
    }

    // Perform the bitwise | 
    aoclsparse_int mm_hint; 
    mm_hint = (A->hint_id & aoclsparse_mm) >> 2;    

    //set if not enabled
    if(!mm_hint)
    {
        A->hint_id = static_cast<aoclsparse_hint_type>(A->hint_id | aoclsparse_mm);  
    }  

    //todo: Any more analysis operations that will help optimize routine later
    //todo: to make use of trans, expected_no_of_calls and matrix descriptor 

    return aoclsparse_status_success;
}
aoclsparse_status aoclsparse_set_2m_hint(aoclsparse_matrix A,
                                                aoclsparse_operation       trans,
                                                const aoclsparse_mat_descr descr,
                                                aoclsparse_int       expected_no_of_calls)
{
    //check descriptor
    if(descr == nullptr)
    {
        return aoclsparse_status_invalid_pointer;
    }    
    // Check index base
    if(descr->base != aoclsparse_index_base_zero)
    {
        // TODO
        return aoclsparse_status_not_implemented;
    }
    if(descr->type != aoclsparse_matrix_type_symmetric)
    {
        // TODO
        return aoclsparse_status_not_implemented;
    }    
    if(trans != aoclsparse_operation_none)
    {
        // TODO
        return aoclsparse_status_not_implemented;
    }    
    // Check sizes
    if(A->m < 0)
    {
        return aoclsparse_status_invalid_size;
    }
    else if(A->n < 0)
    {
        return aoclsparse_status_invalid_size;
    }    
    // Sanity check
    if((A->m == 0 || A->n == 0))
    {
        return aoclsparse_status_invalid_size;
    }    
    if(A->nnz < 0)
    {
        return aoclsparse_status_invalid_size;
    }

    // Check CSR matrix is populated, it not return an error. ToDo: need to handle CSC / COO cases later
    if((A->csr_mat.csr_row_ptr == nullptr) || (A->csr_mat.csr_col_ptr == nullptr) || (A->csr_mat.csr_val == nullptr))
    {
        return aoclsparse_status_invalid_pointer;
    }

    // Perform the bitwise |   
    aoclsparse_int twom_hint; 
    twom_hint = (A->hint_id & aoclsparse_2m) >> 3;  

    //set if not enabled
    if(!twom_hint)
    {
        A->hint_id = static_cast<aoclsparse_hint_type>(A->hint_id | aoclsparse_2m);
    }  

    //todo: Any more analysis operations that will help optimize routine later
    //todo: to make use of trans, expected_no_of_calls and matrix descriptor 

    return aoclsparse_status_success;
}
aoclsparse_status aoclsparse_set_lu_smoother_hint(aoclsparse_matrix A,
                                                aoclsparse_operation       trans,
                                                const aoclsparse_mat_descr descr,
                                                aoclsparse_int       expected_no_of_calls)
{
    //check descriptor
    if(descr == nullptr)
    {
        return aoclsparse_status_invalid_pointer;
    }    
    // Check index base
    if(descr->base != aoclsparse_index_base_zero)
    {
        // TODO
        return aoclsparse_status_not_implemented;
    }
    if(descr->type != aoclsparse_matrix_type_symmetric)
    {
        // TODO
        return aoclsparse_status_not_implemented;
    }    
    if(trans != aoclsparse_operation_none)
    {
        // TODO
        return aoclsparse_status_not_implemented;
    }    
    // Check sizes
    if(A->m < 0)
    {
        return aoclsparse_status_invalid_size;
    }
    else if(A->n < 0)
    {
        return aoclsparse_status_invalid_size;
    }    
    // Sanity check
    if((A->m == 0 || A->n == 0))
    {
        return aoclsparse_status_invalid_size;
    }    
    if(A->nnz < 0)
    {
        return aoclsparse_status_invalid_size;
    }

    // Check CSR matrix is populated, it not return an error. ToDo: need to handle CSC / COO cases later
    if((A->csr_mat.csr_row_ptr == nullptr) || (A->csr_mat.csr_col_ptr == nullptr) || (A->csr_mat.csr_val == nullptr))
    {
        return aoclsparse_status_invalid_pointer;
    }

    // Perform the bitwise | 
    aoclsparse_int ilu_hint; 
    ilu_hint = (A->hint_id & aoclsparse_ilu) >> 4;

    //set if not enabled
    if(!ilu_hint)
    {
        A->hint_id = static_cast<aoclsparse_hint_type>(A->hint_id | aoclsparse_ilu);    
    }  

    //todo: Any more analysis operations that will help optimize routine later
    //todo: to make use of trans, expected_no_of_calls and matrix descriptor 

    return aoclsparse_status_success;
}
