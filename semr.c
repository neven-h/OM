//this program demonstrate the implementation of a semaphore using signals. 
//when the semaphore is initiated with status=1 it send one signal that will be saved(blocked).
//when a thread will try to enter the critical section it will get this signal by calling sem_down, 
//leaving 0 signals available for other threads. 
//when the first thread exits the critical section it send another signal by sem_up.
//thus, another signal is now available and the second thread can enter the critical area.

#include <stdio.h>
#include <pthread.h>
#include <signal.h>
#include <unistd.h>

//global variables
int sig;
sigset_t   set;
// this function initiate the semaphore. if status equal 1, it sends a signal.
void sem_init(int status)
{
	sigemptyset(&set);//initiate the global signal set   
	sigaddset(&set, SIGUSR1);//add SIGUSR1 to the set
	sigprocmask(SIG_BLOCK, &set, NULL );//signals are blocked until a process call sigwait 
	
	
	if (status == 1)
		kill(getpid(),SIGUSR1);
	return;
		
}
//if a thread wants to get into the critical section it calls sem_down.
//sem_down will block the process until a signal is avaliable.
void sem_down()
{
	sigwait(&set,&sig);
}
//when a thread exit the critical section it uses sem_up to send a signal.
//this signal will allow an other thread to enter the critical zone.
void sem_up()
{
	kill(getpid(),SIGUSR1);
}

//this function iterate through a loop and increse a global varibale using a semaphore to prevent race condition.
int glob_val = 0;
void *threadfunction(void *args){
	
	sem_down();
	printf("I Hate\n");
 	for (int i = 0; i < 100000; i++)
	glob_val++ ;
 printf("I Love\n");
	sem_up();
	return NULL;
}
//in order to test the semaphore I created a function that iterate a many times and each time add 1 to a global var.
//usually when 2 threads execute this function at the same time a race condition occurred and the 
//final value of the variable will not be the sum of all the iteration.
//by using a semaphore in this function I preveted the race condition and got the correct result.
int main(){
	
	sem_init(1);
	int retval = 0;
	pthread_t threads[2] = {0};

	retval = pthread_create(&threads[0], NULL, threadfunction, NULL);
	retval = pthread_create(&threads[1], NULL, threadfunction, NULL);

	pthread_join(threads[0], NULL);
	pthread_join(threads[1], NULL);

	printf("glob_val=%d\n",glob_val);

	return 0;
}
