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
    //static float delays[2] = {0.0, 0.0};
    //delay[0] = b[1]*x[1] + b[2]*x[2] - a[1]*y[1] - a[2]*y[2];
    //delay[1] = b[2]*x[1] - a[2]*y[1];
    //delay[0] = 0.0;
    //delay[1] = 0.0;
	DSPF_sp_biquad(x, b, a, delay, y, nx);
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
