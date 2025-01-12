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
#ifndef AOCLSPARSE_ILU0_HPP
#define AOCLSPARSE_ILU0_HPP

#include "aoclsparse.h"
#include "aoclsparse_descr.h"
#include "aoclsparse_types.h"

#if defined(_WIN32) || defined(_WIN64)
//Windows equivalent of gcc c99 type qualifier __restrict__
#define __restrict__ __restrict
#endif

template <typename T>
aoclsparse_status aoclsparse_ilu0_factorization(aoclsparse_int          m,
                                   aoclsparse_int                       n,
                                   aoclsparse_int*  __restrict__        lu_diag_ptr,
                                   aoclsparse_int*  __restrict__        col_idx_mapper,								   
								   T* __restrict__            	csr_val,
                                   const aoclsparse_int* __restrict__   csr_row_ptr,
								   const aoclsparse_int* __restrict__   csr_col_ind)
{
	aoclsparse_status ret = aoclsparse_status_success;
	aoclsparse_int          nrows = m;
	aoclsparse_int i, j, jj, j1, j2, k, mn;
    
    for(i = 0; i < nrows; i++)
    {                    
        j1 = csr_row_ptr[i];
        j2 = csr_row_ptr[i+1];
        //Set mapper
        for(j = j1; j < j2; j++)
        {
            aoclsparse_int col_idx = csr_col_ind[j];
            col_idx_mapper[col_idx] = j;
        }

        //ILU factorization IKJ version k-loop
        for(j = j1; j < j2; j++)
        {
            k = csr_col_ind[j];                  
            if(k < i)
            {                 
                T diag_elem = csr_val[lu_diag_ptr[k]];
                if(diag_elem == 0.0)
                {       
                    ret = aoclsparse_status_internal_error;             
                    return ret;
                }
                csr_val[j] = csr_val[j]/diag_elem; 
                                              
                for(jj = lu_diag_ptr[k] + 1; jj < csr_row_ptr[k + 1]; jj++)
                {
                    int col_idx = csr_col_ind[jj];
                    int jw = col_idx_mapper[col_idx];                    
                    if(jw != 0)
                    {
                        csr_val[jw] = csr_val[jw] - (csr_val[j] * csr_val[jj]);                                                                      
                    }
                }
            }
            else
            {
                break;
            }
        }
        lu_diag_ptr[i] = j;             
        //Error: ro Pivot
        if(k != i || csr_val[j] == 0.0)
        {       
            //ret = i;
            ret = aoclsparse_status_internal_error;             
            return ret;
        }
        //reset mapper
        for(mn = j1; mn < j2; mn++)
        {
            int col_idx = csr_col_ind[mn];
            col_idx_mapper[col_idx] = 0;
        }                                  
    }         
	return ret;
}

template <typename T>
aoclsparse_status aoclsparse_ilu_solve(aoclsparse_int          m,
                                   aoclsparse_int                       n,	
                                   aoclsparse_int* __restrict__          lu_diag_ptr,						   
								   T* __restrict__            	csr_val,								   
								   const aoclsparse_int* __restrict__   row_offsets,
                                   const aoclsparse_int* __restrict__   column_indices,
                                   T* __restrict__                       xv,
                                   const T* __restrict__                 bv)
{
	aoclsparse_status ret = aoclsparse_status_success;
    aoclsparse_int i, k;    

   //Forward Solve
   //Solve L . y = b
   for(i = 0; i < m; i++)
   {
       T sum = bv[i];
       for(k = row_offsets[i]; k < lu_diag_ptr[i]; k++)
       {
           aoclsparse_int col_idx = column_indices[k];
           T temp = 0.0;
           temp = csr_val[k] * xv[col_idx];           
           sum = sum - temp;
       }
       xv[i] = sum;
   } 

   //Backward Solve
   // Solve: U . x = y
   for(i = m - 1; i >= 0; i--)
   {              
       aoclsparse_int diag_idx = lu_diag_ptr[i];
       T diag_elem;
       for(k = lu_diag_ptr[i] + 1; k < row_offsets[i+1]; k++)
       {
           aoclsparse_int col_idx = column_indices[k];
           T temp = 0.0;
           temp = csr_val[k] * xv[col_idx];                       
           xv[i] = xv[i] - temp;
       }    
       diag_elem = csr_val[diag_idx];     
       if(diag_elem != 0.0)
       {
            xv[i] = xv[i]/diag_elem;  
       }   
   }     
	return ret;
}

template <typename T>
aoclsparse_status aoclsparse_ilu0_template(aoclsparse_int               m,
                                   aoclsparse_int                       n,
                                   aoclsparse_int                       nnz,
                                   aoclsparse_int*         lu_diag_ptr,
                                   aoclsparse_int*         col_idx_mapper,
								   bool*                       			is_ilu0_factorized,
								   const aoclsparse_ilu_type*				ilu_fact_type,
								   T*             	        csr_val,
                                   const aoclsparse_int*    csr_row_ptr,
								   const aoclsparse_int*   csr_col_ind,
                                   const T*                diag,
                                   const T*                 approx_inv_diag,                                   								   
                                   T*                  	x,
                                   const T*             	b )
{
	aoclsparse_status ret = aoclsparse_status_success;
	//Perform ILU0 Factorization only once
	if( (*is_ilu0_factorized) == false )
	{
		switch(*ilu_fact_type)
		{
			case aoclsparse_ilu0:
				ret = aoclsparse_ilu0_factorization<T>(m,
											n,
											lu_diag_ptr,
											col_idx_mapper,
											csr_val,
                                            csr_row_ptr,
											csr_col_ind);
				*is_ilu0_factorized = true;			
				break;                           
			default:
				ret = aoclsparse_status_invalid_value;
				break;
		}  		

	}	

    aoclsparse_ilu_solve<T>(m,
                        n,
                        lu_diag_ptr,
                        csr_val,
                        csr_row_ptr,
                        csr_col_ind,                        
                        x,
                        b);
                        
	return ret;
}


#endif // AOCLSPARSE_ILU0_HPP

