/*
 * Synchronization primitives.
 * See synch.h for specifications of the functions.
 */

#include <types.h>
#include <lib.h>
#include <synch.h>
#include <thread.h>
#include <curthread.h>
#include <machine/spl.h>
#include <stdbool.h>
#include <synch.h>

////////////////////////////////////////////////////////////
//
// Semaphore.

struct semaphore *
sem_create(const char *namearg, int initial_count)
{
	struct semaphore *sem;

	assert(initial_count >= 0);

	sem = kmalloc(sizeof(struct semaphore));
	if (sem == NULL) {
		return NULL;
	}

	sem->name = kstrdup(namearg);
	if (sem->name == NULL) {
		kfree(sem);
		return NULL;
	}

	sem->count = initial_count;
	return sem;
}

void
sem_destroy(struct semaphore *sem)
{
	int spl;
	assert(sem != NULL);

	spl = splhigh();
	assert(thread_hassleepers(sem)==0);
	splx(spl);

	/*
	 * Note: while someone could theoretically start sleeping on
	 * the semaphore after the above test but before we free it,
	 * if they're going to do that, they can just as easily wait
	 * a bit and start sleeping on the semaphore after it's been
	 * freed. Consequently, there's not a whole lot of point in 
	 * including the kfrees in the splhigh block, so we don't.
	 */

	kfree(sem->name);
	kfree(sem);
}

void 
P(struct semaphore *sem)
{
	int spl;
	assert(sem != NULL);

	/*
	 * May not block in an interrupt handler.
	 *
	 * For robustness, always check, even if we can actually
	 * complete the P without blocking.
	 */
	assert(in_interrupt==0);

	spl = splhigh();
	while (sem->count==0) {
		thread_sleep(sem);
	}
	assert(sem->count>0);
	sem->count--;
	splx(spl);
}

void
V(struct semaphore *sem)
{
	int spl;
	assert(sem != NULL);
	spl = splhigh();
	sem->count++;
	assert(sem->count>0);
	thread_wakeup(sem);
	splx(spl);
}

////////////////////////////////////////////////////////////
//
// Lock.

struct lock *
lock_create(const char *name)
{
	struct lock *lock;

	lock = kmalloc(sizeof(struct lock));
	if (lock == NULL) {
		return NULL;
	}

	lock->name = kstrdup(name);
	if (lock->name == NULL) {
		kfree(lock);
		return NULL;
	}
	
	// add stuff here as needed

	lock->active = false; // lock initialized not in use yet
	
	lock->lockHolder = NULL;	// No one holds the lock

	return lock;
}

void
lock_destroy(struct lock *lock)
{
	assert(lock != NULL);

	// add stuff here as needed
	int spl;
	spl = splhigh();
	assert(thread_hassleepers(lock) == 0);
	splx(spl);
	
	lock->lockHolder = NULL;

	kfree(lock->name);
	kfree(lock);
}

void
lock_acquire(struct lock *lock)
{
	// Write this

	int spl;
	assert(lock != NULL);

	assert(in_interrupt == 0);

	spl = splhigh();
	
	// While lock is in use, put all threads that try
	// to access lock to sleep (sweet dreams)
	while (lock->active){
		thread_sleep(lock);
	}

	// Make sure lock is off
	assert(!(lock->active));
	
	// If so, turn it on and give it to the thread
	lock->active = true;

	lock->lockHolder = curthread;
	splx(spl);

}

void
lock_release(struct lock *lock)
{
	// Write this
	int spl;
	assert(lock != NULL);
	spl = splhigh();
	lock->active = false;
	assert(!(lock->active));
	lock->lockHolder = NULL;

	// Wake up the next thread that needs to access this lock
	thread_wakeup(lock);
	splx(spl);

}

// Return 1 if the current thread holds the lock, 0 otherwise
int
lock_do_i_hold(struct lock *lock)
{
	// Write this
	if (curthread == lock->lockHolder){
		return 1;
	} else {
		return 0;
	}
}

////////////////////////////////////////////////////////////
//
// CV


struct cv *
cv_create(const char *name)
{
	int spl;
	spl = splhigh();

	
	struct cv *cv;

	cv = kmalloc(sizeof(struct cv));
	if (cv == NULL) {
		return NULL;
	}

	cv->name = kstrdup(name);
	if (cv->name==NULL) {
		kfree(cv);
		return NULL;
	}
	
	cv->sleepingThreads = array_create();
	splx(spl);
	return cv;
}

void
cv_destroy(struct cv *cv)
{
	int spl;
	spl = splhigh();

	assert(cv != NULL);

	// add stuff here as needed
	array_destroy(cv->sleepingThreads);
	
	kfree(cv->name);
	kfree(cv);

	splx(spl);
}

void
cv_wait(struct cv *cv, struct lock *lock)
{
	// Write this
	int spl;
	spl = splhigh();

	// Add current thread to the list of sleeping threads, and release the lock
	lock_release(lock);
	array_add(cv->sleepingThreads,curthread);
	thread_sleep(curthread);
	
	//get it back
	lock_acquire(lock);

	splx(spl);
}

void
cv_signal(struct cv *cv, struct lock *lock)
{
	int spl;
	spl = splhigh();

	// Write this
	thread_wakeup(array_getguy(cv->sleepingThreads, 0));
	
	assert(lock_do_i_hold(lock) == 1);

	array_remove(cv->sleepingThreads, 0);


	splx(spl);
	
	
}

void
cv_broadcast(struct cv *cv, struct lock *lock)
{
	int spl;
	spl = splhigh();

	// Write this
	while (array_getnum(cv->sleepingThreads) > 0){
		thread_wakeup(array_getguy(cv->sleepingThreads, 0));
		array_remove(cv->sleepingThreads, 0);
	}

	splx(spl);
}
