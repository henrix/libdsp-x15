/*
	FFT prototypes (TI DSPLIB)
*/
void DSPF_sp_fftSPxSP(int N, global float *x, global float *w, global float *y,
	unsigned char *brev, int n_min, int offset, int n_max);
void DSPF_sp_ifftSPxSP(int N, global float *x, global float *w, global float *y, 
	unsigned char *brev, int n_min, int offset, int n_max);
void DSPF_sp_fftSPxSP_r2c(int N, global float *x, global float *w, 
	global float *y, unsigned char *brev, int n_min, int offset, int n_max);
void DSPF_sp_ifftSPxSP_c2r (int N, global float *x, global float *w, 
	global float *y, unsigned char *brev, int n_min, int offset, int n_max);
void ocl_DSPF_conv_reverbSPxSP(int N, global float *x, global float *impulse_res,
	global float *wFFT, global float *wIFFT, global float *y)


kernel void ocl_DSPF_sp_fftSPxSP(int N, global float *x, global float *w, 
	global float *y, int n_min, int n_max)
{
	DSPF_sp_fftSPxSP(N, x, w, y, 0, n_min, 0, n_max);
}
kernel void ocl_DSPF_sp_ifftSPxSP(int N, global float *x, global float *w, 
	global float *y, int n_min, int n_max)
{
	DSPF_sp_ifftSPxSP(N, x, w, y, 0, n_min, 0, n_max);
}
/*
kernel void ocl_DSPF_sp_fftSPxSP_r2c(int N, global float *x, global float *w, 
	global float *y, int n_min, int n_max)
{
	DSPF_sp_fftSPxSP_r2c(N, x, w, y, 0, n_min, 0, n_max);
}
kernel void ocl_DSPF_sp_ifftSPxSP_c2r(int N, global float *x, global float *w, 
	global float *y, int n_min, int offset, int n_max)
{
	DSPF_sp_ifftSPxSP_c2r(N, x, w, y, 0, n_min, 0, n_max);
}
*/
kernel void ocl_DSPF_conv_reverbSPxSP(int N, global float *x, global float *impulse_res,
	global float *wFFT, global float *wIFFT, global float *y)
{
	float tmp[2*N];
	DSPF_sp_fftSPxSP(N, x, wFFT, tmp, 0, 2, 0, N);
	for (int i=0; i < 2*N; i++){
		tmp[i] *= impulse_res[i];
	}
	DSPF_sp_ifftSPxSP(N, tmp, wIFFT, y, 0, 2, 0, N);	
}
