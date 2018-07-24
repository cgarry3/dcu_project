extern "C" {
     #include "/home/linaro/work/pynq_libs/PYNQ/sdbuild/packages/libsds/xlnkutils/libxlnk_cma.h"
}

#include <opencv2/opencv.hpp>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>
#include <sys/mman.h>
#include <string.h>
#include <iostream>

#ifndef AXIDMADRIVER_H_
    #define AXIDMADRIVER_H_

    #define MM2S_CONTROL_REGISTER 0x00
    #define MM2S_STATUS_REGISTER 0x04
    #define MM2S_START_ADDRESS 0x18
    #define MM2S_LENGTH 0x28

    #define S2MM_CONTROL_REGISTER 0x30
    #define S2MM_STATUS_REGISTER 0x34
    #define S2MM_DESTINATION_ADDRESS 0x48
    #define S2MM_LENGTH 0x58

    class axiDmaDriver {
        private:
            unsigned int dma_get(unsigned int* dma_virtual_address, int offset); 
            void dma_s2mm_status(unsigned int* dma_virtual_address); 
            void dma_mm2s_status(unsigned int* dma_virtual_address);
            unsigned int dma_set(unsigned int* dma_virtual_address, int offset, unsigned int value); 
            int dma_mm2s_sync(unsigned int* dma_virtual_address); 
            int dma_s2mm_sync(unsigned int* dma_virtual_address); 
            void memdump(void* virtual_address, int byte_count); 
        public:
            void axiDMATransfer(void* pData, void* dData, int transferSize); 
    };

#endif 
