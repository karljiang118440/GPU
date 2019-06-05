#！/bin/bash
export ARCH=arm64
export CROSS_COMPILE=aarch64-linux-gnu-
#export LINUX_S32V234_DIR=/home/stark/bsp14/linux
export LINUX_S32V234_DIR=/media/jcq/study/BSP15.0/linux
#export PATH=$PATH:/home/stark/bsp14/gcc-linaro-4.9-2015.05-x86_64_aarch64-linux-gnu/bin
export PATH=/media/jcq/study/CrossTools/gcc-linaro-4.9-2016.02-x86_64_aarch64-linux-gnu/aarch64-linux-gnu/bin:$PATH
make clean
make
