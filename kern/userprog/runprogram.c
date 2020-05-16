/*
 * Sample/test code for running a user program.  You can use this for
 * reference when implementing the execv() system call. Remember though
 * that execv() needs to do more than this function does.
 */

#include <types.h>
#include <kern/unistd.h>
#include <kern/errno.h>
#include <lib.h>
#include <addrspace.h>
#include <thread.h>
#include <curthread.h>
#include <vm.h>
#include <vfs.h>
#include <test.h>

/*
 * Load program "progname" and start running it in usermode.
 * Does not return except on error.
 *
 * Calls vfs_open on progname and thus may destroy it.
 */
int
runprogram(char *progname, char **args, unsigned long argc)
{
	struct vnode *v;
	vaddr_t entrypoint, stackptr;
	int result;

	/* Open the file. */
	result = vfs_open(progname, O_RDONLY, &v);
	if (result) {
		return result;
	}

	// Cannot assume that it is a new thread, may be called from execv
	// Clear existing content if its not a new thread
	if (curthread->t_vmspace != NULL){
		as_destroy(curthread->t_vmspace);
		curthread->t_vmspace = NULL;
	}

	assert(curthread->t_vmspace == NULL);

	/* Create a new address space. */
	curthread->t_vmspace = as_create();
	if (curthread->t_vmspace==NULL) {
		vfs_close(v);
		return ENOMEM;
	}

	/* Activate it. */
	as_activate(curthread->t_vmspace);

	/* Load the executable. */
	result = load_elf(v, &entrypoint);
	if (result) {
		/* thread_exit destroys curthread->t_vmspace */
		vfs_close(v);
		return result;
	}

	/* Done with the file now. */
	vfs_close(v);

	/* Define the user stack in the address space */
	result = as_define_stack(curthread->t_vmspace, &stackptr);

	if (result) {
		/* thread_exit destroys curthread->t_vmspace */
		return result;
	}

	// copy the argumenets from kernel space to user space
	// allocate kernel space for the arguments
	//char **uargs = (char**)kmalloc(sizeof(char*)*argc);
	
	size_t argLength;
	vaddr_t *argAddr[argc];
	// the last argument should be null indicating the end of the args
	int i = argc - 1;
	int len = 0;
	int stackAlign = 0;
	for(i; i >= 0; i--){
		//kprintf("arg %d: %s\n", i, args[i]);
		// get the length of the string and account for the NULL character 
		len = strlen(args[i])+1;
		stackAlign = len % 4;

		// make sure that the stack aligns to a full word
		if (stackAlign) {
			stackAlign = len + 4 - stackAlign;
		} else {
			stackAlign = len;
		}

		// decrement the stack pointer so that it points to the start of the arg
		stackptr = stackptr - stackAlign;

		// copy the arg onto the stack
		result = copyoutstr(args[i], (userptr_t)stackptr, len, NULL);

		if (result) {
			kprintf("runprogram 105\n");
			return EFAULT;
		}
		argAddr[i] = stackptr;
	}

	// allocate null space to indicate end of arguments
	stackptr = stackptr - 4;
	
	i = argc - 1;

	// put the addresses of the arguments on the stack
	for(i; i >= 0; i--){
		stackptr = stackptr - 4;

		result = copyout(&argAddr[i], (userptr_t)stackptr, sizeof(vaddr_t));
	}

	/* Warp to user mode. */
	md_usermode(argc, (userptr_t)stackptr,
		    stackptr, entrypoint);
	
	/* md_usermode does not return */
	panic("md_usermode returned\n");
	return EINVAL;
}

