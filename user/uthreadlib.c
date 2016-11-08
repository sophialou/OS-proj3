#include "types.h"
#include "stat.h"
#include "user.h"
#include "param.h"
#include "x86.h"

#define PGSIZE (4096)

//void *stackTable[NPROC];
stackTable* stack_t[NPROC];
int numThreads=0;

//Creates a new thread by first allocating a page-aligned user stack, then calling the clone syscall.  
//Returns the pid of the new thread.
int 
thread_create(void (*start_routine)(void*), void* arg){
	void *stack = malloc(PGSIZE*2);
	if((uint)stack % PGSIZE){
     stack = stack + (PGSIZE - (uint)stack % PGSIZE);
	}
	int new_thread_pid = clone(start_routine, &arg, stack);
	if (new_thread_pid < 0){
		free(stack);
	} else {
		stack_t[numThreads]->pid = new_thread_pid;
		stack_t[numThreads]->stack = stack;
		numThreads += 1;
	}

	return new_thread_pid;
}

//Calls join to wait for the thread specified by pid to complete.  
//Cleans up the completed thread's user stack.
int
thread_join(int pid){
	int i;
	int join_pid = join(pid);
	if (join_pid > 0){
		for(i=0; i < NPROC; i++){
			if(stack_t[i] != NULL && stack_t[i]->pid == join_pid) {
				free(stack_t[i]->stack);
				numThreads -= 1;
				stack_t[i] = NULL;
				break;
			}
		}
	}
	return join_pid;
}

//-------------------------------------------------
//                  LOCKS
//-------------------------------------------------

// Acquires the lock pointed to by lock.  If the lock is already held, spin until it becomes available.
void
lock_acquire(lock_t* lock){
	while (xchg(&lock->islocked, 1) == 1)
		; /*  spin-wait */
}

// Release the lock pointed to by lock.
void
lock_release(lock_t* lock){
	lock->islocked = 0;
}

// Initialize the lock pointed to by lock.
void
lock_init(lock_t* lock){
	lock->islocked = 0;
}

//-------------------------------------------------
//           CONDITION VARIABLES
//-------------------------------------------------

// Release the lock pointed to by lock and put the caller to sleep.  Assumes that lock is held when this is called.  When signaled, the thread awakens and reacquires the lock.
void
cv_wait(cond_t* conditionVariable, lock_t* lock){

}

// Wake the threads that are waiting on conditionVariable.
void
cv_signal(cond_t* conditionVariable){

}