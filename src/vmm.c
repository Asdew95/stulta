#include "vmm.h"

union pde kernel_pd[1024] __attribute__((aligned(4096)));

void vmm_init(void)
{
    // Identity map first 4 MB
    kernel_pd[0].huge.pagesize = 1;
    kernel_pd[0].huge.addr_low = 0;
    kernel_pd[0].huge.addr_high = 0;
    kernel_pd[0].huge.readwrite = 1;
    kernel_pd[0].huge.present = 1;

    paging_set_cr3((uint32_t) kernel_pd);
    paging_enable();
}
