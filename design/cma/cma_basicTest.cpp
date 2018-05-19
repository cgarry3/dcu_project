extern "C" {
     #include "/home/linaro/work/pynq_libs/PYNQ/sdbuild/packages/libsds/xlnkutils/libxlnk_cma.h"
}

#include<stdio.h>
#include<stdint.h>

int main()
{
        void* ptr_one;

        uint32_t    size_word  = (uint32_t)sizeof(int);
        uint32_t    cache_mode = (uint32_t)(0);

        // allocating a piece of contiuguous memory
        ptr_one = cma_alloc(size_word, cache_mode);

        if (ptr_one == 0)
        {
                printf("ERROR: Out of memory\n");
                return 1;
        }

        // Getting pointer current value
        unsigned int number = *(unsigned int*)ptr_one;
        printf("Current memory value %d\n", number);

        // Setting pointer to 25
        *((int*)ptr_one)=25;
        number = *(unsigned int*)ptr_one;
        printf("New memory vale %d\n", number);

        // Getting Phy Address
        unsigned long long phy_address = cma_get_phy_addr(ptr_one);
        printf("phy address 0x%llx\n", phy_address);

        // releasing pointer
        cma_free(ptr_one);
        printf("Contiugous memory space is now free up!!!\n");

        return 0;
}
