#include <types.h>
#include <lib.h>
#include <vm.h>
#include <machine/pcb.h>  /* for mips_ramsize */
#include <curthread.h>
#include <syscall.h>

u_int32_t firstfree;   /* first free virtual address; set by start.S */

static u_int32_t firstpaddr;  /* address of first free physical page */
static u_int32_t lastpaddr;   /* one past end of last free physical page */

/*
 * Called very early in system boot to figure out how much physical
 * RAM is available.
 */
void
ram_bootstrap(void)
{
	u_int32_t ramsize;
	
	/* Get size of RAM. */
	ramsize = mips_ramsize();

	/*
	 * This is the same as the last physical address, as long as
	 * we have less than 508 megabytes of memory. If we had more,
	 * various annoying properties of the MIPS architecture would
	 * force the RAM to be discontiguous. This is not a case we 
	 * are going to worry about.
	 */
	if (ramsize > 508*1024*1024) {
		ramsize = 508*1024*1024;
	}

	lastpaddr = ramsize;

	/* 
	 * Get first free virtual address from where start.S saved it.
	 * Convert to physical address.
	 */
	firstpaddr = firstfree - MIPS_KSEG0;

	kprintf("Cpu is MIPS r2000/r3000\n");
	kprintf("%uk physical memory available\n", 
		(lastpaddr-firstpaddr)/1024);
}

/*
 * This function is for allocating physical memory prior to VM
 * initialization.
 *
 * The pages it hands back will not be reported to the VM system when
 * the VM system calls ram_getsize(). If it's desired to free up these
 * pages later on after bootup is complete, some mechanism for adding
 * them to the VM system's page management must be implemented.
 *
 * Note: while the error return value of 0 is a legal physical address,
 * it's not a legal *allocatable* physical address, because it's the
 * page with the exception handlers on it.
 *
 * This function should not be called once the VM system is initialized, 
 * so it is not synchronized.
 */
paddr_t
ram_stealmem(unsigned long npages)
{
	if (coremapCreated < 0){
		u_int32_t size = npages * PAGE_SIZE;
		u_int32_t paddr;

		if (firstpaddr + size > lastpaddr) {
			return 0;
		}

		paddr = firstpaddr;
		firstpaddr += size;

		return paddr;
	} else {
		int i, j, nfree;
		int start = 0;

		if (freeCoremapPages <= 0){
			return 0;
		}

		nfree = 0;
		for (i = 0; i < numCoremapPages; i++){
			if (coremap[i]->valid == 1){
				nfree = 0;
				start = i+1;
			} else {
				nfree++;
			}

			if (nfree == npages){
				for (j = start; j < start + npages; j++){
					coremap[j]->valid = 1;
					coremap[j]->pid = sys_getpid();
					freeCoremapPages--;
					//kprintf("got a page\n");
				}
				coremap[start]->chunkSize = npages;
				assert((coremap[start]->pAddr & PAGE_FRAME) == coremap[start]->pAddr);
				//kprintf("Number of free pages: %d\n", freeCoremapPages);
				return coremap[start]->pAddr;
			}
		}
		return 0;
	}
}

/*
 * This function is intended to be called by the VM system when it
 * initializes in order to find out what memory it has available to
 * manage.
 */
void
ram_getsize(u_int32_t *lo, u_int32_t *hi)
{
	*lo = firstpaddr;
	*hi = lastpaddr;
	firstpaddr = lastpaddr = 0;
}
