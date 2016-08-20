# GSoC 2016 Developement Repository
This repository is used for active development of the project "Improving the CTAG face2|4 Audio Card for BeagleBone-X15" by Henrik Langer (henrix) for Google Summer of Code (GSoC) 2016. The offical project description is published [here](https://summerofcode.withgoogle.com/projects/#5807138232336384).
The C66x DSP library is developed in this repository. The Linux Kernel development can be found [here](https://github.com/henrix/beagle-linux).

> Currently a bug in AM5728 DSP firmware causes kernel crash when recovery mode is enabled in remoteproc debugfs. By disabling remoteproc recovery kernel crash can be avoided (i.e. write "disable" to /sys/kernel/debug/remoteproc/remoteproc2/recovery and the same for remoteproc3).

## Build instructions
### [CTAG face2|4 Audio Card](http://www.creative-technologies.de/linux-based-low-latency-multichannel-audio-system-2/) Driver
1. Compile kernel according to [Kernel_Compilation_Guide in ctag-face-2-4 repo](https://github.com/ctag-fh-kiel/ctag-face-2-4/blob/master/docs/Kernel_Compilation_Guide.md)
2. Transfer zImage, dtbs, modules/firmware and config to BeagleBoard-X15 SD-Card
3. Modify/Add uname_r=4.1.18+ to /boot/uEnv.txt
4. Modify/Add dtb=am57xx-beagle-x15-ctag.dtb to /boot/uEnv.txt
5. Reboot
6. Generate initramfs (update-initramfs -c -k $(uname -r)
7. Reboot (soundcard can be listed with aplay -l)

## Important Sources

### BeagleBoard-X15
* [BB-X15 Github Documentation](https://github.com/beagleboard/beagleboard-x15)
* [BB-X15 elinux Wiki](http://elinux.org/Beagleboard:BeagleBoard-X15)
* [AM572x Datasheet](http://www.ti.com/lit/ds/symlink/am5728.pdf)
* [AM572x Technical Reference Manual](http://www.ti.com/lit/ug/spruhz6f/spruhz6f.pdf)
* [AM572x General Purpose EVM HW User Guide](http://processors.wiki.ti.com/index.php/AM572x_General_Purpose_EVM_HW_User_Guide)

### C66x DSP
* [TI DSP Library Programmer's Reference](http://www.ti.com/lit/ug/sprueb8b/sprueb8b.pdf)
* [TI OpenCL](http://downloads.ti.com/mctools/esd/docs/opencl/index.html)
