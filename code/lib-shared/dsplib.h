/* ======================================================================= */
/* dsplib.h --  Interface header file                 */
/*                                                                         */
/* Rev 0.0.1                                                               */
/*                                                                         */
/* Copyright (C) 2011 Texas Instruments Incorporated - http://www.ti.com/  */
/*                                                                         */
/*                                                                         */
/*  Redistribution and use in source and binary forms, with or without     */
/*  modification, are permitted provided that the following conditions     */
/*  are met:                                                               */
/*                                                                         */
/*    Redistributions of source code must retain the above copyright       */
/*    notice, this list of conditions and the following disclaimer.        */
/*                                                                         */
/*    Redistributions in binary form must reproduce the above copyright    */
/*    notice, this list of conditions and the following disclaimer in the  */
/*    documentation and/or other materials provided with the               */
/*    distribution.                                                        */
/*                                                                         */
/*    Neither the name of Texas Instruments Incorporated nor the names of  */
/*    its contributors may be used to endorse or promote products derived  */
/*    from this software without specific prior written permission.        */
/*                                                                         */
/*  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS    */
/*  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT      */
/*  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR  */
/*  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT   */
/*  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,  */
/*  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT       */
/*  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,  */
/*  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY  */
/*  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT    */
/*  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE  */
/*  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.   */
/*                                                                         */
/* ======================================================================= */
#include <ti/dsplib/src/DSPF_dp_cholesky/DSPF_dp_cholesky.h>
#include <ti/dsplib/src/DSPF_dp_cholesky_cmplx/DSPF_dp_cholesky_cmplx.h>
#include <ti/dsplib/src/DSPF_dp_fftDPxDP/DSPF_dp_fftDPxDP.h>
#include <ti/dsplib/src/DSPF_dp_ifftDPxDP/DSPF_dp_ifftDPxDP.h>
#include <ti/dsplib/src/DSPF_dp_lud/DSPF_dp_lud.h>
#include <ti/dsplib/src/DSPF_dp_lud_cmplx/DSPF_dp_lud_cmplx.h>
#include <ti/dsplib/src/DSPF_dp_lud_inv/DSPF_dp_lud_inv.h>
#include <ti/dsplib/src/DSPF_dp_lud_inv_cmplx/DSPF_dp_lud_inv_cmplx.h>
#include <ti/dsplib/src/DSPF_dp_lud_sol/DSPF_dp_lud_sol.h>
#include <ti/dsplib/src/DSPF_dp_lud_sol_cmplx/DSPF_dp_lud_sol_cmplx.h>
#include <ti/dsplib/src/DSPF_dp_mat_mul_gemm/DSPF_dp_mat_mul_gemm.h>
#include <ti/dsplib/src/DSPF_dp_mat_submat_copy/DSPF_dp_mat_submat_copy.h>
#include <ti/dsplib/src/DSPF_dp_mat_trans/DSPF_dp_mat_trans.h>
#include <ti/dsplib/src/DSPF_dp_qrd/DSPF_dp_qrd.h>
#include <ti/dsplib/src/DSPF_dp_qrd_cmplx/DSPF_dp_qrd_cmplx.h>
#include <ti/dsplib/src/DSPF_dp_svd/DSPF_dp_svd.h>
#include <ti/dsplib/src/DSPF_dp_svd_cmplx/DSPF_dp_svd_cmplx.h>
#include <ti/dsplib/src/DSPF_sp_autocor/DSPF_sp_autocor.h>
#include <ti/dsplib/src/DSPF_sp_biquad/DSPF_sp_biquad.h>
#include <ti/dsplib/src/DSPF_sp_bitrev_cplx/DSPF_sp_bitrev_cplx.h>
#include <ti/dsplib/src/DSPF_sp_cholesky/DSPF_sp_cholesky.h>
#include <ti/dsplib/src/DSPF_sp_cholesky_cmplx/DSPF_sp_cholesky_cmplx.h>
#include <ti/dsplib/src/DSPF_sp_convol/DSPF_sp_convol.h>
#include <ti/dsplib/src/DSPF_sp_dotprod/DSPF_sp_dotprod.h>
#include <ti/dsplib/src/DSPF_sp_dotp_cplx/DSPF_sp_dotp_cplx.h>
#include <ti/dsplib/src/DSPF_sp_erand/DSPF_sp_erand.h>
#include <ti/dsplib/src/DSPF_sp_fftSPxSP/DSPF_sp_fftSPxSP.h>
#include <ti/dsplib/src/DSPF_sp_fftSPxSP_r2c/DSPF_sp_fftSPxSP_r2c.h>
#include <ti/dsplib/src/DSPF_sp_fircirc/DSPF_sp_fircirc.h>
#include <ti/dsplib/src/DSPF_sp_fir_cplx/DSPF_sp_fir_cplx.h>
#include <ti/dsplib/src/DSPF_sp_fir_gen/DSPF_sp_fir_gen.h>
#include <ti/dsplib/src/DSPF_sp_fir_r2/DSPF_sp_fir_r2.h>
#include <ti/dsplib/src/DSPF_sp_ifftSPxSP/DSPF_sp_ifftSPxSP.h>
#include <ti/dsplib/src/DSPF_sp_ifftSPxSP_c2r/DSPF_sp_ifftSPxSP_c2r.h>
#include <ti/dsplib/src/DSPF_sp_iirlat/DSPF_sp_iirlat.h>
#include <ti/dsplib/src/DSPF_sp_iir/DSPF_sp_iir.h>
#include <ti/dsplib/src/DSPF_sp_lms/DSPF_sp_lms.h>
#include <ti/dsplib/src/DSPF_sp_lud/DSPF_sp_lud.h>
#include <ti/dsplib/src/DSPF_sp_lud_cmplx/DSPF_sp_lud_cmplx.h>
#include <ti/dsplib/src/DSPF_sp_lud_inv/DSPF_sp_lud_inv.h>
#include <ti/dsplib/src/DSPF_sp_lud_inv_cmplx/DSPF_sp_lud_inv_cmplx.h>
#include <ti/dsplib/src/DSPF_sp_lud_sol/DSPF_sp_lud_sol.h>
#include <ti/dsplib/src/DSPF_sp_lud_sol_cmplx/DSPF_sp_lud_sol_cmplx.h>
#include <ti/dsplib/src/DSPF_sp_mat_mul/DSPF_sp_mat_mul.h>
#include <ti/dsplib/src/DSPF_sp_mat_mul_cplx/DSPF_sp_mat_mul_cplx.h>
#include <ti/dsplib/src/DSPF_sp_mat_mul_gemm/DSPF_sp_mat_mul_gemm.h>
#include <ti/dsplib/src/DSPF_sp_mat_mul_gemm_cplx/DSPF_sp_mat_mul_gemm_cplx.h>
#include <ti/dsplib/src/DSPF_sp_mat_submat_copy/DSPF_sp_mat_submat_copy.h>
#include <ti/dsplib/src/DSPF_sp_mat_submat_copy_cplx/DSPF_sp_mat_submat_copy_cplx.h>
#include <ti/dsplib/src/DSPF_sp_mat_trans/DSPF_sp_mat_trans.h>
#include <ti/dsplib/src/DSPF_sp_mat_trans_cplx/DSPF_sp_mat_trans_cplx.h>
#include <ti/dsplib/src/DSPF_sp_maxidx/DSPF_sp_maxidx.h>
#include <ti/dsplib/src/DSPF_sp_maxval/DSPF_sp_maxval.h>
#include <ti/dsplib/src/DSPF_sp_minerr/DSPF_sp_minerr.h>
#include <ti/dsplib/src/DSPF_sp_minval/DSPF_sp_minval.h>
#include <ti/dsplib/src/DSPF_sp_nrand/DSPF_sp_nrand.h>
#include <ti/dsplib/src/DSPF_sp_qrd/DSPF_sp_qrd.h>
#include <ti/dsplib/src/DSPF_sp_qrd_cmplx/DSPF_sp_qrd_cmplx.h>
#include <ti/dsplib/src/DSPF_sp_svd/DSPF_sp_svd.h>
#include <ti/dsplib/src/DSPF_sp_svd_cmplx/DSPF_sp_svd_cmplx.h>
#include <ti/dsplib/src/DSPF_sp_urand/DSPF_sp_urand.h>
#include <ti/dsplib/src/DSPF_sp_vecadd/DSPF_sp_vecadd.h>
#include <ti/dsplib/src/DSPF_sp_vecmul/DSPF_sp_vecmul.h>
#include <ti/dsplib/src/DSPF_sp_vecrecip/DSPF_sp_vecrecip.h>
#include <ti/dsplib/src/DSPF_sp_vecsum_sq/DSPF_sp_vecsum_sq.h>
#include <ti/dsplib/src/DSPF_sp_w_vec/DSPF_sp_w_vec.h>
#include <ti/dsplib/src/DSP_add16/DSP_add16.h>
#include <ti/dsplib/src/DSP_add32/DSP_add32.h>
#include <ti/dsplib/src/DSP_autocor/DSP_autocor.h>
#include <ti/dsplib/src/DSP_bexp/DSP_bexp.h>
#include <ti/dsplib/src/DSP_blk_eswap16/DSP_blk_eswap16.h>
#include <ti/dsplib/src/DSP_blk_eswap32/DSP_blk_eswap32.h>
#include <ti/dsplib/src/DSP_blk_eswap64/DSP_blk_eswap64.h>
#include <ti/dsplib/src/DSP_blk_move/DSP_blk_move.h>
#include <ti/dsplib/src/DSP_dotprod/DSP_dotprod.h>
#include <ti/dsplib/src/DSP_dotp_sqr/DSP_dotp_sqr.h>
#include <ti/dsplib/src/DSP_fft16x16r/DSP_fft16x16r.h>
#include <ti/dsplib/src/DSP_fft16x16/DSP_fft16x16.h>
#include <ti/dsplib/src/DSP_fft16x16_imre/DSP_fft16x16_imre.h>
#include <ti/dsplib/src/DSP_fft16x32/DSP_fft16x32.h>
#include <ti/dsplib/src/DSP_fft32x32s/DSP_fft32x32s.h>
#include <ti/dsplib/src/DSP_fft32x32/DSP_fft32x32.h>
#include <ti/dsplib/src/DSP_firlms2/DSP_firlms2.h>
#include <ti/dsplib/src/DSP_fir_cplx/DSP_fir_cplx.h>
#include <ti/dsplib/src/DSP_fir_cplx_hM4X4/DSP_fir_cplx_hM4X4.h>
#include <ti/dsplib/src/DSP_fir_gen/DSP_fir_gen.h>
#include <ti/dsplib/src/DSP_fir_gen_hM17_rA8X8/DSP_fir_gen_hM17_rA8X8.h>
#include <ti/dsplib/src/DSP_fir_r4/DSP_fir_r4.h>
#include <ti/dsplib/src/DSP_fir_r8/DSP_fir_r8.h>
#include <ti/dsplib/src/DSP_fir_r8_h16/DSP_fir_r8_h16.h>
#include <ti/dsplib/src/DSP_fir_r8_h24/DSP_fir_r8_h24.h>
#include <ti/dsplib/src/DSP_fir_r8_h8/DSP_fir_r8_h8.h>
#include <ti/dsplib/src/DSP_fir_r8_hM16_rM8A8X8/DSP_fir_r8_hM16_rM8A8X8.h>
#include <ti/dsplib/src/DSP_fir_sym/DSP_fir_sym.h>
#include <ti/dsplib/src/DSP_fltoq15/DSP_fltoq15.h>
#include <ti/dsplib/src/DSP_ifft16x16/DSP_ifft16x16.h>
#include <ti/dsplib/src/DSP_ifft16x16_imre/DSP_ifft16x16_imre.h>
#include <ti/dsplib/src/DSP_ifft16x32/DSP_ifft16x32.h>
#include <ti/dsplib/src/DSP_ifft32x32/DSP_ifft32x32.h>
#include <ti/dsplib/src/DSP_iir/DSP_iir.h>
#include <ti/dsplib/src/DSP_iir_lat/DSP_iir_lat.h>
#include <ti/dsplib/src/DSP_iir_ss/DSP_iir_ss.h>
#include <ti/dsplib/src/DSP_mat_mul/DSP_mat_mul.h>
#include <ti/dsplib/src/DSP_mat_mul_cplx/DSP_mat_mul_cplx.h>
#include <ti/dsplib/src/DSP_mat_trans/DSP_mat_trans.h>
#include <ti/dsplib/src/DSP_maxidx/DSP_maxidx.h>
#include <ti/dsplib/src/DSP_maxval/DSP_maxval.h>
#include <ti/dsplib/src/DSP_minerror/DSP_minerror.h>
#include <ti/dsplib/src/DSP_minval/DSP_minval.h>
#include <ti/dsplib/src/DSP_mul32/DSP_mul32.h>
#include <ti/dsplib/src/DSP_neg32/DSP_neg32.h>
#include <ti/dsplib/src/DSP_q15tofl/DSP_q15tofl.h>
#include <ti/dsplib/src/DSP_recip16/DSP_recip16.h>
#include <ti/dsplib/src/DSP_urand16/DSP_urand16.h>
#include <ti/dsplib/src/DSP_urand32/DSP_urand32.h>
#include <ti/dsplib/src/DSP_vecsumsq/DSP_vecsumsq.h>
#include <ti/dsplib/src/DSP_w_vec/DSP_w_vec.h>
