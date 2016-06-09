# GSoC 2016 Developement Repository 
This repository is used for active development of the project "Improving the CTAG face2|4 Audio Card for BeagleBone-X15" by Henrik Langer (henrix) for Google Summer of Code (GSoC) 2016. The offical project description is published [here](https://summerofcode.withgoogle.com/projects/#5807138232336384).
The C66x DSP library is developed in this repository. The Linux Kernel development can be found [here](https://github.com/henrix/beagle-linux).

## Build Process
1. Create SD-Card with default TI Linux SDK image (see [TI Guide](http://processors.wiki.ti.com/index.php/Processor_SDK_Linux_Getting_Started_Guide) for detailed information)
2. Boot SD-Card
3. Install sshfs: ```sudo apt-get install sshfs```
4. Mount root file system of SBC via sshfs (e.g. ```sshfs root@<IP-Adress of BeagleBoard-X15>:/ ~/mnt```)
5. Set enviroment variable to target root dir: ```export $TARGET_ROOTDIR=~/mnt```
6. Switch to code directory in this repo
7. Compile project and demo: ```make```

## Important Sources

### BeagleBoard-X15
* [BB-X15 Github Documentation](https://github.com/beagleboard/beagleboard-x15)
* [BB-X15 elinux Wiki](http://elinux.org/Beagleboard:BeagleBoard-X15)

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
