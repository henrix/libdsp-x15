# GSoC 2016 Developement Repository
This repository is used for active development of the project "Improving the CTAG face2|4 Audio Card for BeagleBone-X15" by Henrik Langer (henrix) for Google Summer of Code (GSoC) 2016. The offical project description is published [here](https://summerofcode.withgoogle.com/projects/#5807138232336384).
The C66x DSP library is developed in this repository. The Linux Kernel development can be found [here](https://github.com/henrix/beagle-linux).

## Modified SD-Card image with soundcard driver, library and demo app is available here
TBD

> Currently a bug in AM5728 DSP firmware causes kernel crash when recovery mode is enabled in remoteproc debugfs. By disabling remoteproc recovery kernel crash can be avoided (i.e. write "disable" to /sys/kernel/debug/remoteproc/remoteproc2/recovery and the same for remoteproc3).

## Build Process
### Prerequisites
#### JACK (no-dbus) (took from [SuperCollider Guide](https://supercollider.github.io/development/building-beagleboneblack.html))
1. git clone git://github.com/jackaudio/jack2.git --depth 1
2. cd jack2
3. ./waf configure --alsa
4. ./waf build
5. sudo ./waf install
6. sudo ldconfig
7. sudo nano /etc/security/limits.conf #and add the following two lines at the end
  * @audio - memlock 256000
  * @audio - rtprio 75
8. Reboot

#### QT and qmake
1. sudo apt-get install qt5-default qt5-qmake

#### QCustomPlot for realtime plots
1. wget http://www.qcustomplot.com/release/1.3.2/QCustomPlot.tar.gz
2. mdkir <this_repo>/code/qcustomplot
3. tar -xvzf QCustomPlot.tar.gz -C qcustomplot

### [CTAG face2|4 Audio Card](http://www.creative-technologies.de/linux-based-low-latency-multichannel-audio-system-2/) Driver
1. Compile kernel according to [Kernel_Compilation_Guide in ctag-face-2-4 repo](https://github.com/ctag-fh-kiel/ctag-face-2-4/blob/master/docs/Kernel_Compilation_Guide.md)
2. Transfer zImage, dtbs, modules/firmware and config to BeagleBoard-X15 SD-Card
3. Modify/Add uname_r=4.1.18+ to /boot/uEnv.txt
4. Modify/Add dtb=am57xx-beagle-x15-ctag.dtb to /boot/uEnv.txt
5. Reboot
6. Generate initramfs (update-initramfs -c -k $(uname -r)
7. Reboot (soundcard can be listed with aplay -l)

### C66x Audio / Signal Library
1. Clone this repo on BeagleBoard-X15
2. Switch to <this_repo>/code/lib
3. make

### QT Demo App
1. Clone this repo
2. Switch to <this_repo>/code/test-libaudio-x15
3. qmake test-libaudio-x15.pro
4. make

## Important Sources

### BeagleBoard-X15
* [BB-X15 Github Documentation](https://github.com/beagleboard/beagleboard-x15)
* [BB-X15 elinux Wiki](http://elinux.org/Beagleboard:BeagleBoard-X15)
* [AM572x Datasheet](http://www.ti.com/lit/ds/symlink/am5728.pdf)
* [AM572x Technical Reference Manual](http://www.ti.com/lit/ug/spruhz6f/spruhz6f.pdf)
* [AM572x General Purpose EVM HW User Guide](http://processors.wiki.ti.com/index.php/AM572x_General_Purpose_EVM_HW_User_Guide)

### TI Processor SDK

* [Processor SDK AM57x - RTOS and Linux](http://www.ti.com/tool/PROCESSOR-SDK-AM57X)
* [Processor SDK - Linux Software Developer's Guide](http://processors.wiki.ti.com/index.php/Processor_SDK_Linux_Software_Developer%E2%80%99s_Guide)
* [AM57x Sitara Processors Training Series](https://training.ti.com/am57x-sitara-processors-training-series)

### C66x DSP
* [TI DSP Library Programmer's Reference](http://www.ti.com/lit/ug/sprueb8b/sprueb8b.pdf)
* [TI OpenCL](http://downloads.ti.com/mctools/esd/docs/opencl/index.html)

### GStreamer C66x plugin template:
* [Processor Training: Multimedia](http://processors.wiki.ti.com/index.php/Processor_Training:_Multimedia)
* [GStreamer C66x Plugin Template](http://git.ti.com/processor-sdk/gst-plugin-dsp66)
* [GStreamer Application Development Manual](https://gstreamer.freedesktop.org/data/doc/gstreamer/head/manual/html/index.html)
