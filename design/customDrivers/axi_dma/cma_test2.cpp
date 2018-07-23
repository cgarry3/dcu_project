#include<stdio.h>
#include<stdint.h>
#include "/home/linaro/work/pynq_libs/PYNQ/sdbuild/packages/libsds/xlnkutils/libxlnk_cma.h"

int main()
{
	void* ptr_one;

	uint32_t    size_word  = (uint32_t)sizeof(int);
    uint32_t    cache_mode = (uint32_t)(0);
	
	ptr_one = cma_alloc(size_word, cache_mode);

	if (ptr_one == 0)
	{
		printf("ERROR: Out of memory\n");
		return 1;
	}

	//*ptr_one = 25;
	//printf("%d\n", *ptr_one);

	cma_free(ptr_one);

	//unsigned long phy_address = cma_get_phy_addr(ptr_one)
	//printf("phy address %lu\n", *ptr_one);

	return 0;
}
