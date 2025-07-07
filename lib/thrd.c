#include <pthread.h> 
#include <stdlib.h> 
#include <stdio.h>
#include <termios.h>
#include <unistd.h>

#include "thrd.h"

// thread_list 
int threads_capacity = 1;
int threads_index    = 0;
pthread_t *threads   = NULL;

int thrd_arr_reinit(){
	if(threads == NULL){
		threads = calloc(threads_capacity,sizeof(pthread_t));
		if(threads != NULL){
			return 0;
		}
	} else {
		// threads is pre-initialized 
		if(threads_index == threads_capacity){
			threads_capacity *= 2;
			pthread_t *temp = realloc(threads,threads_capacity*sizeof(pthread_t));
			if(temp != NULL){
				threads = temp;
				return 0;
			}
		}
	}
	// something went wrong
	return 1;
}	


// create and run a thread 
int cs_thrd(void *thread_function(void *) ){
	if(thrd_arr_reinit()==0){
		if (pthread_create(
					&threads[threads_index],
					NULL,
					thread_function,
					NULL
					)==0){
			threads_index++;
			return 0;
		}
	} 
	return 1;
}


// kill all threads 
int ka_thrd(){
	for(int i=0;i<threads_index;i++){
		pthread_cancel(threads[i]);
		// loop through and cancel all 
		// threads
	}
	
	
	// free the threads array
	free(threads);
	return 0;
}

int nogchr() {	
  static struct termios oldt, newt;
  tcgetattr(STDIN_FILENO, &oldt);
  newt = oldt;
  newt.c_lflag &= ~(ICANON | ECHO);
  tcsetattr(STDIN_FILENO, TCSANOW, &newt);
  int ch = getchar();
  tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
  return ch;
}
