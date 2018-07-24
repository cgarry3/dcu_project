using namespace cv;


unsigned int dma_get(unsigned int* dma_virtual_address, int offset) {
    return dma_virtual_address[offset>>2];
}

void dma_s2mm_status(unsigned int* dma_virtual_address) {
    unsigned int status = dma_get(dma_virtual_address, S2MM_STATUS_REGISTER);
    printf("Stream to memory-mapped status (0x%08x@0x%02x):", status, S2MM_STATUS_REGISTER);
    if (status & 0x00000001) printf(" halted"); else printf(" running");
    if (status & 0x00000002) printf(" idle");
    if (status & 0x00000008) printf(" SGIncld");
    if (status & 0x00000010) printf(" DMAIntErr");
    if (status & 0x00000020) printf(" DMASlvErr");
    if (status & 0x00000040) printf(" DMADecErr");
    if (status & 0x00000100) printf(" SGIntErr");
    if (status & 0x00000200) printf(" SGSlvErr");
    if (status & 0x00000400) printf(" SGDecErr");
    if (status & 0x00001000) printf(" IOC_Irq");
    if (status & 0x00002000) printf(" Dly_Irq");
    if (status & 0x00004000) printf(" Err_Irq");
    printf("\n");
}

void dma_mm2s_status(unsigned int* dma_virtual_address) {
    unsigned int status = dma_get(dma_virtual_address, MM2S_STATUS_REGISTER);
    printf("Memory-mapped to stream status (0x%08x@0x%02x):", status, MM2S_STATUS_REGISTER);
    if (status & 0x00000001) printf(" halted"); else printf(" running");
    if (status & 0x00000002) printf(" idle");
    if (status & 0x00000008) printf(" SGIncld");
    if (status & 0x00000010) printf(" DMAIntErr");
    if (status & 0x00000020) printf(" DMASlvErr");
    if (status & 0x00000040) printf(" DMADecErr");
    if (status & 0x00000100) printf(" SGIntErr");
    if (status & 0x00000200) printf(" SGSlvErr");
    if (status & 0x00000400) printf(" SGDecErr");
    if (status & 0x00001000) printf(" IOC_Irq");
    if (status & 0x00002000) printf(" Dly_Irq");
    if (status & 0x00004000) printf(" Err_Irq");
    printf("\n");
}

unsigned int dma_set(unsigned int* dma_virtual_address, int offset, unsigned int value) {
    dma_virtual_address[offset>>2] = value;
}


int dma_mm2s_sync(unsigned int* dma_virtual_address) {
    unsigned int mm2s_status =  dma_get(dma_virtual_address, MM2S_STATUS_REGISTER);
    while(!(mm2s_status & 1<<12) || !(mm2s_status & 1<<1) ){
        dma_s2mm_status(dma_virtual_address);
        dma_mm2s_status(dma_virtual_address);

        mm2s_status =  dma_get(dma_virtual_address, MM2S_STATUS_REGISTER);
    }
}

int dma_s2mm_sync(unsigned int* dma_virtual_address) {
    unsigned int s2mm_status = dma_get(dma_virtual_address, S2MM_STATUS_REGISTER);
    while(!(s2mm_status & 1<<12) || !(s2mm_status & 1<<1)){
        dma_s2mm_status(dma_virtual_address);
        dma_mm2s_status(dma_virtual_address);

        s2mm_status = dma_get(dma_virtual_address, S2MM_STATUS_REGISTER);
    }
}

void memdump(void* virtual_address, int byte_count) {
    char *p = (char*)virtual_address;
    int offset;
    for (offset = 0; offset < byte_count; offset++) {
        printf("%02x", p[offset]);
        if (offset % 4 == 3) { printf(" "); }
    }
    printf("\n");
}


void axiDMATransfer(void* pData, void* dData, int transferSize) {
    int dh = open("/dev/mem", O_RDWR | O_SYNC); // Open /dev/mem which represents the whole physical memory
    unsigned int* virtual_address = (unsigned int*)mmap(NULL, 65535, PROT_READ | PROT_WRITE, MAP_SHARED, dh, 0x40400000); // Memory map AXI Lite register block

    // ----------------------------------------------
    //  Setting up pointers
    // ----------------------------------------------

    // size of image
    uint32_t    size_word  = (uint32_t)transferSize;
    uint32_t    cache_mode = (uint32_t)(0);

    // cma pointers
    void* src;
    void* dest;

    // create space for source and destination
    src = cma_alloc(sizeof(unsigned char)*width*height*3, cache_mode);
    dest = cma_alloc(sizeof(unsigned char)*width*height*3, cache_mode);

    // copy image to cma
    memcpy(src, pData, sizeof(unsigned char)*width*height*3);

    // check if allocated correctly
    if (src == 0 || dest == 0)
    {
            printf("ERROR: Out of memory\n");
            return 1;
    }



    // ----------------------------------------------
    //  Getting phy addresses
    // ----------------------------------------------

    unsigned long long phy_address1 = cma_get_phy_addr(src);
    unsigned long long phy_address2 = cma_get_phy_addr(dest);


    // --------------------------------------------------
    //  DMA transfer
    // --------------------------------------------------

    dma_set(virtual_address, S2MM_CONTROL_REGISTER, 4);
    dma_set(virtual_address, MM2S_CONTROL_REGISTER, 4);
    dma_s2mm_status(virtual_address);
    dma_mm2s_status(virtual_address);

    dma_set(virtual_address, S2MM_CONTROL_REGISTER, 0);
    dma_set(virtual_address, MM2S_CONTROL_REGISTER, 0);
    dma_s2mm_status(virtual_address);
    dma_mm2s_status(virtual_address);

    dma_set(virtual_address, S2MM_DESTINATION_ADDRESS, phy_address2); // Write destination address
    dma_s2mm_status(virtual_address);

    dma_set(virtual_address, MM2S_START_ADDRESS, phy_address1); // Write source address
    dma_mm2s_status(virtual_address);

    dma_set(virtual_address, S2MM_CONTROL_REGISTER, 0xf001);
    dma_s2mm_status(virtual_address);

    dma_set(virtual_address, MM2S_CONTROL_REGISTER, 0xf001);
    dma_mm2s_status(virtual_address);

    dma_set(virtual_address, S2MM_LENGTH, static_cast<int>(size_word));
    //dma_set(virtual_address, S2MM_LENGTH, 150);
    dma_s2mm_status(virtual_address);
    std::cout << "length: " << dma_get(virtual_address, S2MM_LENGTH) << std::endl;
    
    dma_set(virtual_address, MM2S_LENGTH, static_cast<int>(size_word));
    dma_mm2s_status(virtual_address);

    // not sure why sync is broken!!!
    //dma_mm2s_sync(virtual_address);
    //dma_s2mm_sync(virtual_address); // If this locks up make sure all memory ranges are assigned under Address Editor!

    dma_s2mm_status(virtual_address);
    dma_mm2s_status(virtual_address);


    // --------------------------------------------------
    //  Save output image
    // --------------------------------------------------

    // copy image from cma to image file
    memcpy(dData, dest, sizeof(unsigned char)*width*height*3);


    // releasing pointer
    cma_free(src);
    cma_free(dest);
    printf("AXI DMA transfer complete!!!\n");

}
