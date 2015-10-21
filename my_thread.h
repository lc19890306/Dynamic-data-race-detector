//my_thread.h
//written by Chang Liu
//head file of a simple data race detector using lockset algorithm
//for the course project of CS/ECE 5510 Multiprocessor Programming

//pthread library
#include <pthread.h>

//macro for advanced c++ version
#ifdef _MY_THREAD_H
#define _MY_THREAD_H
#ifdef __cplusplus 

extern "C" {

#endif

//used in thread routine function
//initialize the thread
void my_thread_start();

//used in thread routine function
//do nothing at all right now
//backup for future update
void my_thread_end();

//used in main function
//initialize indexes for each array and some flags
//initialize pthread_mutex_t object used in the program
void my_main_start();

//used in main function
//check the flag and make some printouts
void my_main_end();

//used in main function
//register and initialize each shared variable
//obsolete in advanced version (merged into read and write functions)
void my_obj_reg(int *data);

//used in thread routine function
//maintain each lockset based on different status of shared variables
void my_obj_read(int *data);

//used in thread routine function
//maintain each lockset based on different status of shared variables
void my_obj_write(int *data);

//used in main function
//record each lock (pthread_mutex_t object)
//initialize each lock (ptherad_mutex_t object)
int my_mutex_init(pthread_mutex_t *mutex, const pthread_mutexattr_t *attr);

//used in main function
//destroy each lock (pthread_mutex_t object)
int my_mutex_destroy(pthread_mutex_t *mutex);

//used in thread routine function
//maintain each lockset for different threads
//call pthread_mutex_lock
int my_mutex_lock(pthread_mutex_t *mutex);

//used in thread routine function
//maintain each lockset for different threads
//call pthread_mutex_unlock
int my_mutex_unlock(pthread_mutex_t *mutex);

//used in main function
//register each thread (pthread_t object)
//create each thread
int my_thread_create(pthread_t *thread, const pthread_attr_t *attr, void*(*start_routine)(void*), void *arg);

//used in main function
//do nothing but join all the threads right now
//backup for advanced version
int my_thread_join(pthread_t thread, void **value_ptr);

//used in thread routine function
//find the index of specified lock (pthread_mutex_t object)
int my_find_mutex(pthread_mutex_t *mutex);

//used in thread routine function
//find the index of specified thread (pthread_t object)
int my_find_thread(pthread_t thread);

//used in thread routine function
//find the index of specified shared variable
int my_find_var(int *data);

#ifdef __cplusplus 

}
#endif

#endif
