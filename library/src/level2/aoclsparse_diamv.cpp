/* ************************************************************************
 * Copyright (c) 2020-2021 Advanced Micro Devices, Inc.All rights reserved.
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
#include "aoclsparse_diamv.hpp"

/*
 *===========================================================================
 *   C wrapper
 * ===========================================================================
 */
extern "C" aoclsparse_status aoclsparse_sdiamv(aoclsparse_operation       trans,
                                   const float*              alpha,
                                   aoclsparse_int             m,
                                   aoclsparse_int             n,
                                   aoclsparse_int             nnz,
                                   const float*              dia_val,
                                   const aoclsparse_int*      dia_offset,
                                   aoclsparse_int      dia_num_diag,
                                   const aoclsparse_mat_descr descr,
                                   const float*             x,
                                   const float*            beta,
                                   float*                   y )
{
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
    if(descr->type != aoclsparse_matrix_type_general)
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
    if(m < 0)
    {
        return aoclsparse_status_invalid_size;
    }
    else if(n < 0)
    {
        return aoclsparse_status_invalid_size;
    }
    else if(dia_num_diag < 0)
    {
        return aoclsparse_status_invalid_size;
    }

    // Sanity check
    if((m == 0 || n == 0) && dia_num_diag != 0)
    {
        return aoclsparse_status_invalid_size;
    }

    // Quick return if possible
    if(m == 0 || n == 0 || dia_num_diag == 0)
    {
        return aoclsparse_status_success;
    }

    // Check pointer arguments
    if(dia_val == nullptr)
    {
        return aoclsparse_status_invalid_pointer;
    }
    else if(dia_offset == nullptr)
    {
        return aoclsparse_status_invalid_pointer;
    }
    else if(x == nullptr)
    {
        return aoclsparse_status_invalid_pointer;
    }
    else if(y == nullptr)
    {
        return aoclsparse_status_invalid_pointer;
    }

    return aoclsparse_diamv_template(*alpha,
                            m,
                            n,
                            nnz,
                            dia_val,
                            dia_offset,
                            dia_num_diag,
                            x,
                            *beta,
                            y);
}

extern "C" aoclsparse_status aoclsparse_ddiamv(aoclsparse_operation       trans,
                                   const double*              alpha,
                                   aoclsparse_int             m,
                                   aoclsparse_int             n,
                                   aoclsparse_int             nnz,
                                   const double*              dia_val,
                                   const aoclsparse_int*      dia_offset,
                                   aoclsparse_int      dia_num_diag,
                                   const aoclsparse_mat_descr descr,
                                   const double*             x,
                                   const double*            beta,
                                   double*                   y )
{
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
    if(descr->type != aoclsparse_matrix_type_general)
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
    if(m < 0)
    {
        return aoclsparse_status_invalid_size;
    }
    else if(n < 0)
    {
        return aoclsparse_status_invalid_size;
    }
    else if(dia_num_diag < 0)
    {
        return aoclsparse_status_invalid_size;
    }

    // Sanity check
    if((m == 0 || n == 0) && dia_num_diag != 0)
    {
        return aoclsparse_status_invalid_size;
    }

    // Quick return if possible
    if(m == 0 || n == 0 || dia_num_diag == 0)
    {
        return aoclsparse_status_success;
    }

    // Check pointer arguments
    if(dia_val == nullptr)
    {
        return aoclsparse_status_invalid_pointer;
    }
    else if(dia_offset == nullptr)
    {
        return aoclsparse_status_invalid_pointer;
    }
    else if(x == nullptr)
    {
        return aoclsparse_status_invalid_pointer;
    }
    else if(y == nullptr)
    {
        return aoclsparse_status_invalid_pointer;
    }

    return aoclsparse_diamv_template(*alpha,
                            m,
                            n,
                            nnz,
                            dia_val,
                            dia_offset,
                            dia_num_diag,
                            x,
                            *beta,
                            y);
}
