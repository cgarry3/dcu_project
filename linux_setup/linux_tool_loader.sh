# -------------------------------------------------------------------------
#                 Linux Tool loader
#
#  Author:       Cathal Garry
#  Description:  This bash script is used to load the necessary files and
#                git repos needed to build a linux image for a zybo 
#                platform. The root system that is used is Linaro
#                
# -------------------------------------------------------------------------

#!/bin/bash

# Custom viviado install area

EXPORT VIVADO_INSTALL_AREA=/home/cgarry/Xilinx/Vivado/2017.04/

echo "###### Getting Zybo Setup Files ######"

wget -P ./ "https://raw.githubusercontent.com/ucb-bar/fpga-zynq/master/zybo/src/xml/ZYBO_zynq_def.xml"
wget -P ./ "https://raw.githubusercontent.com/Digilent/ZYBO/master/Resources/XDC/ZYBO_Master.xdc"

echo "###### Building U-Boot Image ######"

## cloning U-Boot repo
git clone https://github.com/SDU-Embedded/u-boot-xlnx.git

## use arm-xilinx-linux-gnueabi- in version of Xilinx SDK less than 2017.2
export CROSS_COMPILE=arm-linux-gnueabihf-
source $VIVADO_INSTALL_AREA/settings64.sh

## Building image
cd u-boot-xlnx/
make zynq_zybo_config
make
cd tools
export PATH=`pwd`:$PATH
cd ..
mv u-boot u-boot.elf

echo "###### Building Linux Kernel Image ######"

## cloning linux kernel repo
git clone https://github.com/Xilinx/linux-xlnx.git

## Building kernel image
cd linux-xlnx/
make ARCH=arm xilinx_zynq_defconfig

## exit this unless you want to make changes
make ARCH=arm menuconfig
make ARCH=arm UIMAGE_LOADADDR=0x8000 uImage

## optional build kernal modules
make ARCH=arm modules
sudo make ARCH=arm INSTALL_MOD_PATH=mnt/ modules_install

echo "###### Cloning Device tree ######"
git clone https://github.com/SDU-Embedded/device-tree-xlnx.git
