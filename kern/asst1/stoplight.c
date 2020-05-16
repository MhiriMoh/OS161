/* 
 * stoplight.c
 *
 * 31-1-2003 : GWA : Stub functions created for CS161 Asst1.
 *
 * NB: You can use any synchronization primitives available to solve
 * the stoplight problem in this file.
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
#include <synch.h>


/*
 *
 * Constants
 *
 */

/*
 * Number of cars created.
 */

#define NCARS 20


/*
 *
 * Function Definitions
 *
 */

static const char *directions[] = { "N", "E", "S", "W" };

static const char *msgs[] = {
        "approaching:",
        "region1:    ",
        "region2:    ",
        "region3:    ",
        "leaving:    "
};

struct lock *NW, *NE, *SW, *SE, *mutex;
struct lock *N, *S, *E, *W, *done;
int carsInIntersection = 0;
int carsRunning = 0;
//struct cv *3CarsInIntersection;

/* use these constants for the first parameter of message */
enum { APPROACHING, REGION1, REGION2, REGION3, LEAVING };

static void
message(int msg_nr, int carnumber, int cardirection, int destdirection)
{
        kprintf("%s car = %2d, direction = %s, destination = %s\n",
                msgs[msg_nr], carnumber,
                directions[cardirection], directions[destdirection]);
}
 
/*
 * gostraight()
 *
 * Arguments:
 *      unsigned long cardirection: the direction from which the car
 *              approaches the intersection.
 *      unsigned long carnumber: the car id number for printing purposes.
 *
 * Returns:
 *      nothing.
 *
 * Notes:
 *      This function should implement passing straight through the
 *      intersection from any direction.
 *      Write and comment this function.
 */

static
void
gostraight(unsigned long cardirection,
           unsigned long carnumber,  unsigned long destdirection)
{
    // Check if there are 3 or more cars in the intersection
    // If so, entering will cause deadlock, so wait

    lock_acquire(mutex);
    while (carsInIntersection >= 3){
        lock_release(mutex);
        thread_yield();
    }
    carsInIntersection++;
    lock_release(mutex);

        switch(cardirection){
            // NW - SW
            case 0:
                // do not release your position, until car has moved to next pos
                lock_acquire(NW);
                lock_release(N);
                message(1, carnumber, cardirection, destdirection);
                lock_acquire(SW);
                message(2, carnumber, cardirection, destdirection);
                lock_release(NW);
                message(4, carnumber, cardirection, destdirection); 

                // car has left intersection
                lock_acquire(mutex);
                carsInIntersection--;
                lock_release(SW);
                lock_release(mutex); 

                
            break;
            // NE - NW
            case 1:
                lock_acquire(NE);
                lock_release(E);
                message(1, carnumber, cardirection, destdirection);
                lock_acquire(NW);
                message(2, carnumber, cardirection, destdirection);
                lock_release(NE);
                message(4, carnumber, cardirection, destdirection); 

                lock_acquire(mutex);
                carsInIntersection--;
                lock_release(NW);
                lock_release(mutex); 

                
            break;
            // SE - NE
            case 2:
                lock_acquire(SE);
                lock_release(S);
                message(1, carnumber, cardirection, destdirection);
                lock_acquire(NE);
                message(2, carnumber, cardirection, destdirection);
                lock_release(SE);
                message(4, carnumber, cardirection, destdirection); 
                
                lock_acquire(mutex);
                carsInIntersection--;
                lock_release(NE);
                lock_release(mutex); 

            break;
            // SW - SE
            case 3:
                lock_acquire(SW);
                lock_release(W);
                message(1, carnumber, cardirection, destdirection);
                lock_acquire(SE);
                message(2, carnumber, cardirection, destdirection);
                lock_release(SW);
                message(4, carnumber, cardirection, destdirection); 
                

                lock_acquire(mutex);
                carsInIntersection--;
                lock_release(SE);
                lock_release(mutex); 

            break;
            default:
            break;
        }
}


/*
 * turnleft()
 *
 * Arguments:
 *      unsigned long cardirection: the direction from which the car
 *              approaches the intersection.
 *      unsigned long carnumber: the car id number for printing purposes.
 *
 * Returns:
 *      nothing.
 *
 * Notes:
 *      This function should implement making a left turn through the 
 *      intersection from any direction.
 *      Write and comment this function.
 */

static
void
turnleft(unsigned long cardirection,
         unsigned long carnumber, unsigned long destdirection)
{
    // Check if there are 3 or more cars in the intersection
    // If so, entering will cause deadlock, so wait

    lock_acquire(mutex);
    while (carsInIntersection >= 3){
        lock_release(mutex);
        thread_yield();
    }
    carsInIntersection++;
    lock_release(mutex);
        switch(cardirection){
            // NW - SW - SE
            case 0:
                // dont give up current position until next position is aquired
                lock_acquire(NW);
                lock_release(N);
                message(1, carnumber, cardirection, destdirection);
                lock_acquire(SW);
                message(2, carnumber, cardirection, destdirection);
                lock_release(NW);
                
                lock_acquire(SE);
                message(3, carnumber, cardirection, destdirection);
                lock_release(SW);
                message(4, carnumber, cardirection, destdirection);

                // car has left the intersection
                lock_acquire(mutex);
                carsInIntersection--;
                lock_release(SE);
                lock_release(mutex); 

                
            break;
            // NE - NW - SW
            case 1:
                lock_acquire(NE);
                lock_release(E);
                message(1, carnumber, cardirection, destdirection);
                lock_acquire(NW);
                message(2, carnumber, cardirection, destdirection);
                lock_release(NE);
                lock_acquire(SW);
                message(3, carnumber, cardirection, destdirection);
                lock_release(NW);
                message(4, carnumber, cardirection, destdirection); 

                lock_acquire(mutex);
                carsInIntersection--;
                lock_release(SW);
                lock_release(mutex); 

                
            break;
            // SE - NE - NW
            case 2:
                lock_acquire(SE);
                lock_release(S);
                message(1, carnumber, cardirection, destdirection);
                lock_acquire(NE);
                message(2, carnumber, cardirection, destdirection);
                lock_release(SE);
                
                lock_acquire(NW);
                message(3, carnumber, cardirection, destdirection);
                lock_release(NE);
                message(4, carnumber, cardirection, destdirection); 

                lock_acquire(mutex);
                carsInIntersection--;
                lock_release(NW);
                lock_release(mutex); 

                
            break;
            // SW - SE - NE
            case 3:
                lock_acquire(SW);
                lock_release(W);
                message(1, carnumber, cardirection, destdirection);
                lock_acquire(SE);
                message(2, carnumber, cardirection, destdirection);
                lock_release(SW);
                
                lock_acquire(NE);
                message(3, carnumber, cardirection, destdirection);
                lock_release(SE);
                message(4, carnumber, cardirection, destdirection); 

                lock_acquire(mutex);
                carsInIntersection--;
                lock_release(NE);
                lock_release(mutex); 

                
            break;
            default:
            break;
        }
}


/*
 * turnright()
 *
 * Arguments:
 *      unsigned long cardirection: the direction from which the car
 *              approaches the intersection.
 *      unsigned long carnumber: the car id number for printing purposes.
 *
 * Returns:
 *      nothing.
 *
 * Notes:
 *      This function should implement making a right turn through the 
 *      intersection from any direction.
 *      Write and comment this function.
 */

static
void
turnright(unsigned long cardirection,
          unsigned long carnumber,  unsigned long destdirection)
{
    // right turns cannot deadlock

        switch(cardirection){
            // NW
            case 0:

                lock_acquire(NW);
                lock_release(N);
                message(1, carnumber, cardirection, destdirection);
                message(4, carnumber, cardirection, destdirection); 
                lock_release(NW);
            break;
            // NE
            case 1:
                lock_acquire(NE);
                lock_release(E);
                message(1, carnumber, cardirection, destdirection);
                message(4, carnumber, cardirection, destdirection); 
                lock_release(NE);
            break;
            // SE
            case 2:
                lock_acquire(SE);
                lock_release(S);
                message(1, carnumber, cardirection, destdirection);
                message(4, carnumber, cardirection, destdirection); 
                lock_release(SE);
            break;
            // SW
            case 3:
                lock_acquire(SW);
                lock_release(W);
                message(1, carnumber, cardirection, destdirection);
                message(4, carnumber, cardirection, destdirection); 
                lock_release(SW);
            break;
            default:
            break;
        }

}


/*
 * approachintersection()
 *
 * Arguments: 
 *      void * unusedpointer: currently unused.
 *      unsigned long carnumber: holds car id number.
 *
 * Returns:
 *      nothing.
 *
 * Notes:
 *      Change this function as necessary to implement your solution. These
 *      threads are created by createcars().  Each one must choose a direction
 *      randomly, approach the intersection, choose a turn randomly, and then
 *      complete that turn.  The code to choose a direction randomly is
 *      provided, the rest is left to you to implement.  Making a turn
 *      or going straight should be done by calling one of the functions
 *      above.
 */
 
static
void
approachintersection(void * unusedpointer,
                     unsigned long carnumber)
{
        int cardirection;
        int destdirection;
        /*
         * Avoid unused variable and function warnings.
         */

        (void) unusedpointer;
        (void) carnumber;
	(void) gostraight;
	(void) turnleft;
	(void) turnright;

        /*
         * cardirection is set randomly.
         */

    // directions[] = { "N", "E", "S", "W" };

        cardirection = random() % 4;

        destdirection = random() % 3;


        switch(cardirection){
            // Starting from north
            case 0:
                switch(destdirection){
                    // Going East
                    case 0:
                        message(0, carnumber, 0, 1);    // approaching 
                        // aquire the north entrance lock, so that no overtaking
                        // can occur
                        lock_acquire(N);     
                        turnleft(0,carnumber,1);
                           // leaving
                    break;
                    // Going South
                    case 1:
                        message(0, carnumber, 0, 2);
                        lock_acquire(N);
                        gostraight(0,carnumber,2);
                        
                    break;
                    // Going west
                    case 2:
                        message(0, carnumber, 0, 3);
                        lock_acquire(N);
                        turnright(0,carnumber,3);
                       
                    break;
                    default:
                    break;
                }
                break;

            // starting east
            case 1:
                switch(destdirection){
                    // Going north
                    case 0:

                        message(0, carnumber, 1, 0);    // approaching

                        // aquire the east entrance lock, so that no overtaking
                        // can occur
                        lock_acquire(E);
                        turnright(1,carnumber,0);
                           // leaving
                    break;
                    // Going South
                    case 1:
                        message(0, carnumber, 1, 2);
                        lock_acquire(E);
                        turnleft(1,carnumber,2);
                        
                    break;
                    // Going west
                    case 2:
                        message(0, carnumber, 1, 3);
                        lock_acquire(E);
                        gostraight(1,carnumber,3);
                        
                    break;
                    default:
                    break;
                }
            break;

            // Starting from south
            case 2:
                switch(destdirection){
                    // Going north
                    case 0:
                        message(0, carnumber, 2, 0);    // approaching

                        // aquire the south entrance lock, so that no overtaking
                        // can occur
                        lock_acquire(S);
                        gostraight(2,carnumber,0);
                           // leaving
                    break;
                    // Going east
                    case 1:
                        message(0, carnumber, 2, 1);
                        lock_acquire(S);
                        turnright(2,carnumber,1);
                        
                    break;
                    // Going west
                    case 2:
                        message(0, carnumber, 2, 3);
                        lock_acquire(S);
                        turnleft(2,carnumber,3);
                        
                    break;
                    default:
                    break;
                }
                break;

            // starting west
            case 3:
                switch(destdirection){
                    // Going north
                    case 0:
                        message(0, carnumber, 3, 0);    // approaching

                        // aquire the west entrance lock, so that no overtaking
                        // can occur
                        lock_acquire(W);
                        turnleft(3,carnumber,0);
                           // leaving
                    break;
                    // Going east
                    case 1:
                        message(0, carnumber, 3, 1);
                        lock_acquire(W);    
                        gostraight(3,carnumber,1);
                       
                    break;
                    // Going south
                    case 2:
                        message(0, carnumber, 3, 2);
                        lock_acquire(W);
                        turnright(3,carnumber,2);
                   
                    break;
                    default:
                    break;
                }
            break;
            default:
            break;
        }

        lock_acquire(done);
        carsRunning--;
        lock_release(done);

}


/*
 * createcars()
 *
 * Arguments:
 *      int nargs: unused.
 *      char ** args: unused.
 *
 * Returns:
 *      0 on success.
 *
 * Notes:
 *      Driver code to start up the approachintersection() threads.  You are
 *      free to modiy this code as necessary for your solution.
 */

int
createcars(int nargs,
           char ** args)
{
        int index, error;

        /*
         * Avoid unused variable warnings.
         */

        (void) nargs;
        (void) args;

        NW = lock_create("North-West");
        NE = lock_create("North-East");
        SW = lock_create("South-West");
        SE = lock_create("South-East");

        N = lock_create("North-Enter");
        S = lock_create("South-Enter");
        E = lock_create("East-Enter");
        W = lock_create("West-Enter");

        mutex = lock_create("mutex");
        done = lock_create("done");

        //3CarsInIntersection = cv_create("3CarsInIntersection");

        carsInIntersection = 0;

        /*
         * Start NCARS approachintersection() threads.
         */


        for (index = 0; index < NCARS; index++) {

                carsRunning++;

                error = thread_fork("approachintersection thread",
                                    NULL,
                                    index,
                                    approachintersection,
                                    NULL
                                    );

                /*
                 * panic() on error.
                 */

                if (error) {
                        
                        panic("approachintersection: thread_fork failed: %s\n",
                              strerror(error)
                              );
                }
        }

        while (carsRunning != 0){
            thread_yield();
        }

        lock_destroy(N);
        lock_destroy(S);
        lock_destroy(E);
        lock_destroy(W);

        lock_destroy(NW);
        lock_destroy(NE);
        lock_destroy(SW);
        lock_destroy(SE);

        lock_destroy(mutex);
        lock_destroy(done);

        return 0;
}
