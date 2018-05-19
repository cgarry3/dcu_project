extern "C" {
     #include "/home/linaro/work/pynq_libs/PYNQ/sdbuild/packages/libsds/xlnkuti$
}

#include<stdio.h>
#include<stdint.h>

int main()
{
        void* ptr_one;
        void* ptr_two;

        uint32_t    size_word  = (uint32_t)sizeof(int);
        uint32_t    cache_mode = (uint32_t)(0);

        // allocating a piece of contiuguous memory
        ptr_one = cma_alloc(size_word, cache_mode);
        ptr_two = cma_alloc(size_word, cache_mode);
        
        if (ptr_one == 0 || ptr_two == 0)
        {
                printf("ERROR: Out of memory\n");
                return 1;
        }

        // Getting pointer current value
        unsigned int number = *(unsigned int*)ptr_one;
        unsigned int number2 = *(unsigned int*)ptr_two;
        printf("Current memory value of pointer one %d\n", number);
        printf("Current memory value of pointer two %d\n", number2);
        
        // Setting pointer to 25
        *((int*)ptr_one)=25;
        number = *(unsigned int*)ptr_one;
        printf("New memory value of pointer one %d\n", number);
        printf("Current memory value of pointer two %d\n", number2);

        // copying int value from pointer one to pointer two
        memcpy((unsigned char*)ptr_two, (unsigned char*)ptr_one, size_word);
        printf("value copied from pointer one to pointer two!!!\n");
        number = *(unsigned int*)ptr_two;
        printf("latest memory value %d\n", number);


        // Getting Phy Address
        unsigned long long phy_address = cma_get_phy_addr(ptr_one);
        printf("phy address 0x%llx\n", phy_address);
        phy_address = cma_get_phy_addr(ptr_two);
        printf("phy address 0x%llx\n", phy_address);

        // releasing pointer
        cma_free(ptr_one);
        cma_free(ptr_two);
        printf("Contiugous memory space is now free up!!!\n");

        return 0;
}
