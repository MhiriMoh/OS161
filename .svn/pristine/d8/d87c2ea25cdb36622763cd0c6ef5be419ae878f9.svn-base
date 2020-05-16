/*
 * catlock.c
 *
 * 30-1-2003 : GWA : Stub functions created for CS161 Asst1.
 *
 * NB: Please use LOCKS/CV'S to solve the cat syncronization problem in 
 * this file.
 */


/*
 * 
 * Includes
 *
 */

#include <types.h>
#include <lib.h>
#include <test.h>
#include <thread.h>
#include <stdbool.h>


/*
 * 
 * Constants
 *
 */

/*
 * Number of food bowls.
 */

#define NFOODBOWLS 2

/*
 * Number of cats.
 */

#define NCATS 6

/*
 * Number of mice.
 */

#define NMICE 2




struct lock *bowlsEat[NFOODBOWLS]; //locks for the bowls
struct lock *tempBlock; // a temporary blocker, that prevents other threads from hijacking global vars
int catsEating = 0; //maintain count of cats eating
int miceEating = 0; //maintain count of mice eating
bool bowl1 = false; // whether to eat from bowl 1 or bowl 2, prioritizes use of bowl 1
int hungryAnimals =0;
struct lock *doneEating;



/*
 * 
 * Function Definitions
 * 
 */

/* who should be "cat" or "mouse" */
static void
lock_eat(const char *who, int num, int bowl, int iteration)
{
        kprintf("%s: %d starts eating: bowl %d, iteration %d\n", who, num, 
                bowl, iteration);
        clocksleep(1);
        kprintf("%s: %d ends eating: bowl %d, iteration %d\n", who, num, 
                bowl, iteration);
}

/*
 * catlock()
 *
 * Arguments:
 *      void * unusedpointer: currently unused.
 *      unsigned long catnumber: holds the cat identifier from 0 to NCATS -
 *      1.
 *
 * Returns:
 *      nothing.
 *
 * Notes:
 *      Write and comment this function using locks/cv's.
 *
 */

static
void
catlock(void * unusedpointer, 
        unsigned long catnumber)
{
        /*
         * Avoid unused variable warnings.
         */
		int bowl, iteration;

		for(iteration = 0; iteration <= 3; iteration ++){ // 4 iterations of eating
			
			lock_acquire(tempBlock); //temporarily block other threads

			if(catsEating < 2 && miceEating == 0){ // only eat if the number of cats doesnt exceed the number of bowls and there are no mice eating

				catsEating++; 

				bowl = (catnumber % 2) +1;

				lock_release(tempBlock); //realease the tempblocker, let other threads access critical code

				lock_acquire(bowlsEat[bowl-1]); // lockout the bowl to be used
				lock_eat("cat", catnumber, bowl, iteration); //eat using the bowl
				lock_release(bowlsEat[bowl-1]);//release the bowl that was used


				lock_acquire(tempBlock);
				
				catsEating--;
				lock_release(tempBlock);

			}

			else{
				lock_release(tempBlock); // if the cat cannot eat right now then release the lock
				iteration--; // if not eating dont iterate
			}

			thread_yield(); //let anther animal have a turn

		}

		lock_acquire(doneEating);
		hungryAnimals--;
		lock_release(doneEating);
        (void) unusedpointer;
       
}
	

/*
 * mouselock()
 *
 * Arguments:
 *      void * unusedpointer: currently unused.
 *      unsigned long mousenumber: holds the mouse identifier from 0 to 
 *              NMICE - 1.
 *
 * Returns:
 *      nothing.
 *
 * Notes:
 *      Write and comment this function using locks/cv's.
 *
 */

static
void
mouselock(void * unusedpointer,
          unsigned long mousenumber)
{
        /*
         * Avoid unused variable warnings.
         */
        

		int bowl, iteration;

		for(iteration = 0; iteration <= 3; iteration ++){
			
			lock_acquire(tempBlock);

			if(catsEating == 0 && miceEating < 2){

				miceEating++;

				bowl = (mousenumber % 2) + 1;

				lock_release(tempBlock);

				lock_acquire(bowlsEat[bowl-1]);
				lock_eat("mouse", mousenumber, bowl, iteration);
				lock_release(bowlsEat[bowl-1]);

				lock_acquire(tempBlock);


				miceEating--;
				lock_release(tempBlock);
			}

			else{
				lock_release(tempBlock);
				iteration--;
			}

			thread_yield();

		}

		lock_acquire(doneEating);
		hungryAnimals--;
		lock_release(doneEating);

        (void) unusedpointer;

}


/*
 * catmouselock()
 *
 * Arguments:
 *      int nargs: unused.
 *      char ** args: unused.
 *
 * Returns:
 *      0 on success.
 *
 * Notes:
 *      Driver code to start up catlock() and mouselock() threads.  Change
 *      this code as necessary for your solution.
 */

int
catmouselock(int nargs,
             char ** args)
{
        int index, error;
   
        /*
         * Avoid unused variable warnings.
         */

        (void) nargs;
        (void) args;
   
        /*
         * Start NCATS catlock() threads.
         */

        // create the locks
        tempBlock = lock_create("tempBlock");
        assert(tempBlock !=NULL);

        for(index = 0; index< NFOODBOWLS; index++){
        	bowlsEat[index] = lock_create("bowlLock");
        	assert(bowlsEat[index] != NULL);
        }

        doneEating = lock_create("doneEating");
        assert(doneEating != NULL);

        for (index = 0; index < NCATS; index++) {
           
            lock_acquire(doneEating);
			hungryAnimals++;
			lock_release(doneEating);

                error = thread_fork("catlock thread", 
                                    NULL, 
                                    index, 
                                    catlock, 
                                    NULL
                                    );
                
                /*
                 * panic() on error.
                 */

                if (error) {
                 
                        panic("catlock: thread_fork failed: %s\n", 
                              strerror(error)
                              );
                }
        }

        /*
         * Start NMICE mouselock() threads.
         */

        for (index = 0; index < NMICE; index++) {
   				
        		lock_acquire(doneEating);
				hungryAnimals++;
				lock_release(doneEating);

                error = thread_fork("mouselock thread", 
                                    NULL, 
                                    index, 
                                    mouselock, 
                                    NULL
                                    );
      
                /*
                 * panic() on error.
                 */

                if (error) {
         
                        panic("mouselock: thread_fork failed: %s\n", 
                              strerror(error)
                              );
                }
        }

        while(hungryAnimals != 0)
        	thread_yield();

        lock_destroy(tempBlock);

        for(index = 0; index< NFOODBOWLS; index++){
        	lock_destroy(bowlsEat[index]);
        }

        lock_destroy(doneEating);

        return 0;
}

/*
 * End of catlock.c
 */
