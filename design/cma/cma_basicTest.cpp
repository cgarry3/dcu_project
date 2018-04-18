#include<stdio.h>
#include "/home/linaro/work/pynq_libs/PYNQ/sdbuild/packages/libsds/xlnkutils/lib_xlnkcma.h"

int main()
{
	int *ptr_one;

	ptr_one = (int *)cma_alloc(sizeof(int));

	if (ptr_one == 0)
	{
		printf("ERROR: Out of memory\n");
		return 1;
	}

	*ptr_one = 25;
	printf("%d\n", *ptr_one);

	cma_free(ptr_one);

	//unsigned long phy_address = cma_get_phy_addr(ptr_one)
	//printf("phy address %lu\n", *ptr_one);

	return 0;
}
