# libdsp-x15 - C66x DSP library focusing on audio applications for BeagleBoard-x15 (AM5728) 
This project has been started within [Google Summer of Code 2016](https://summerofcode.withgoogle.com/archive/2016/projects/5351212496977920/).
The library was created to offload typical audio processing operations to the C66x DSPs (integrated in AM5728 SoC of TI). 
Currently the library offers the following signal opertions:
* Fast Fourier Transform (FFT)
* Inverse Fast Fourier Transform (IFFT)
* Biquad Filter (2nd order IIR filter (often used in audio applications due to stability))

Please check out the [wiki](https://github.com/henrix/libdsp-x15/wiki) for complete DSP library documentation.

Moreover, within this GSoC project, the [CTAG face2|4 Multichannel Audio Card](https://hackaday.io/project/9634-linux-based-low-latency-multichannel-audio-system) driver has been succesfully ported to AM5728 SoC. The driver (compatible with BeagleBone Black/green and BeagleBoard-X15) has been merged in the offical BeagleBoard kernel and is ready to use by default in the BeagleBone images since end of 2016.
