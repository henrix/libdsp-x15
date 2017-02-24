/*
	Prototypes (TI DSPLIB)
*/
void DSPF_sp_fftSPxSP(int N, global float *x, global float *w, global float *y,
	unsigned char *brev, int n_min, int offset, int n_max);
void DSPF_sp_fftSPxSP_r2c(int N, global float *x, global float *w,
	global float *y, unsigned char *brev, int n_min, int offset, int n_max);
void DSPF_sp_ifftSPxSP(int N, global float *x, global float *w, global float *y,
	unsigned char *brev, int n_min, int offset, int n_max);
void DSPF_sp_biquad(global float *restrict x, global float *b, global float *a,
	global float *delay, global float *restrict y, const int nx);
void DSPF_sp_fircirc(global const float *x, global float *h,
	global float *restrict y, int index, int csize, int nh, int ny);
void DSPF_sp_fir_cplx(global const float *x, global const float *h,
	global float *restrict y, int nh, int nr);
void DSPF_sp_fir_gen(global const float *restrict x,
	global const float *restrict h, global float *restrict r, int nh, int nr);
void DSPF_sp_fir_r2(global const float *x, global const float *h,
	global float *restrict r, const int nh, const int nr);
void DSPF_sp_iir(global float *restrict y1, global const float *x,
	global float *restrict y2, global const float *hb,
	global const float *ha, int nr);
void DSPF_sp_iirlat(global const float *x, int nx,
	global const float *restrict k, int nk, global float *restrict b,
	global float *restrict y);

__constant unsigned char brev[64] = {
    0x0, 0x20, 0x10, 0x30, 0x8, 0x28, 0x18, 0x38,
    0x4, 0x24, 0x14, 0x34, 0xc, 0x2c, 0x1c, 0x3c,
    0x2, 0x22, 0x12, 0x32, 0xa, 0x2a, 0x1a, 0x3a,
    0x6, 0x26, 0x16, 0x36, 0xe, 0x2e, 0x1e, 0x3e,
    0x1, 0x21, 0x11, 0x31, 0x9, 0x29, 0x19, 0x39,
    0x5, 0x25, 0x15, 0x35, 0xd, 0x2d, 0x1d, 0x3d,
    0x3, 0x23, 0x13, 0x33, 0xb, 0x2b, 0x1b, 0x3b,
    0x7, 0x27, 0x17, 0x37, 0xf, 0x2f, 0x1f, 0x3f
};

/*
	Wrappers
*/
kernel void ocl_DSPF_sp_fftSPxSP(int N, global float *x, global float *w,
	global float *y, int n_min, int n_max)
{
	DSPF_sp_fftSPxSP(N, x, w, y, 0, n_min, 0, n_max);
}

kernel void ocl_DSPF_sp_fftSPxSP_r2c(int N, global float *x, global float *w,
	global float *y, int n_min, int n_max)
{
	DSPF_sp_fftSPxSP_r2c(N, x, w, y, 0, n_min, 0, n_max);
}

kernel void ocl_DSPF_sp_ifftSPxSP(int N, global float *x, global float *w,
	global float *y, int n_min, int n_max)
{
	DSPF_sp_ifftSPxSP(N, x, w, y, 0, n_min, 0, n_max);
}
kernel void ocl_DSPF_sp_biquad(global float *restrict x, global float *b,
	global float *a, global float *delay, global float *restrict y, const int nx)
{
	DSPF_sp_biquad(x, b, a, delay, y, nx);
}
kernel void ocl_foo()
{
	printf("DSP kernel foo() executed\n");
}
kernel void ocl_DSPF_sp_fircirc (global const float *x, global float *h,
	global float *restrict y, int index, int csize, int nh, int ny)
{
	DSPF_sp_fircirc(x, h, y, index, csize, nh, ny);
}
kernel void ocl_DSPF_sp_fir_cplx(global const float *x, global const float *h,
	global float *restrict y, int nh, int nr)
{
	DSPF_sp_fir_cplx(x, h, y, nh, nr);
}
kernel void ocl_DSPF_sp_fir_gen(global const float *restrict x,
	global const float *restrict h, global float *restrict r, int nh, int nr)
{
	DSPF_sp_fir_gen(x, h, r, nh, nr);
}
kernel void ocl_DSPF_sp_fir_r2(global const float *x, global const float *h,
	global float *restrict r, const int nh, const int nr)
{
	DSPF_sp_fir_r2(x, h, r, nh, nr);
}
kernel void ocl_DSPF_sp_iir(global float *restrict y1, global const float *x,
	global float *restrict y2, global const float *hb,
	global const float *ha, int nr)
{
	DSPF_sp_iir(y1, x, y2, hb, ha, nr);
}
kernel void ocl_DSPF_sp_iirlat(global const float *x, int nx,
	global const float *restrict k, int nk, global float *restrict b,
	global float *restrict y)
{
	DSPF_sp_iirlat(x, nx, k, nk, b, y);
}

/*
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
*/
