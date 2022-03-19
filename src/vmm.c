#include "vmm.h"

union pde kernel_pd[1024] __attribute__((aligned(4096)));

void vmm_init(void)
{
    // Recursive paging
    kernel_pd[1023].small.addr = ((uint32_t) kernel_pd - 0xfe000000) >> 12;
    kernel_pd[1023].small.present = 1;
}
