# TI OpenCL Guide

## Memory
* Memory types:
	* DDR (shared between host and DSPs)
	* MSMC (shared between DSP cores)
	* L2 SRAM (only accessible from single DSP core)
* Alternate Host malloc/free extensions
	* void *__malloc_ddr(size_t size)
	* void __free_ddr(void *p)
	* void *__malloc_msmc(size_t size)
	* void __free_msmc(void *p)
	