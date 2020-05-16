#ifndef _SYSCALL_H_
#define _SYSCALL_H_

#include <thread.h>

/*
 * Prototypes for IN-KERNEL entry points for system call implementations.
 */
struct lock *readLock;

int sys_reboot(int code);

int sys_write(int fd, const void *buf, size_t nbytes, int32_t *returnValue);

int sys_read(int fd, void *buf, size_t buflen, int32_t *returnValue);

pid_t sys_fork(struct trapframe*, int* retval); 
	
pid_t sys_getpid(void);

pid_t sys_waitpid(pid_t pid, int *statuts, int options, int* retval, int where);

//additional functions
void removeExitedThreads(pid_t pId); 

void md_forkentry(struct trapframe *tf, unsigned long aSpace);

struct processInfo* getProcess(pid_t pId);

#endif /* _SYSCALL_H_ */
