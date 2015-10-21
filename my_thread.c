//my_thread.c
//written by Chang Liu
//c file to support my_thread head file
//function definitions for a simple data race detector using lockset algorithm
//for the course project of CS/ECE 5510 Multiprocessor Programming

//standard io
#include <stdio.h>

//standard library
#include <stdlib.h>

//pthread library
#include <pthread.h>

//my_thread head file
#include "my_thread.h"

//printout for debug information
#define DBG_PRINT //printf("thread %lu is calling %s, file %s, line %d\n", (unsigned long int)pthread_self(), __FUNCTION__, __FILE__, __LINE__)

//printout for each shared variable
#define PRINT //printf("shared_var_array[%d]: state: %d; C(v): %d; LH(%lu): %d; WLH(%lu): %d\n", tmp_var_index, shared_var_array[tmp_var_index].state, shared_var_array[tmp_var_index].locks_candidate, pthread_self(), thread_array[tmp_thread_index].locks_held, pthread_self(), thread_array[tmp_thread_index].write_locks_held);

//printout for all shared variables (provisional)
#define PRINT1 //printf("shared_var_array[0]: state: %d; C(v): %d; LH(%lu): %d; WLH(%lu): %d\nshared_var_array[1]: state: %d; C(v): %d; LH(%lu): %d; WLH(%lu): %d\n", shared_var_array[0].state, shared_var_array[0].locks_candidate, pthread_self(), thread_array[tmp_thread_index].locks_held, pthread_self(), thread_array[tmp_thread_index].write_locks_held, shared_var_array[1].state, shared_var_array[1].locks_candidate, pthread_self(), thread_array[tmp_thread_index].locks_held, pthread_self(), thread_array[tmp_thread_index].write_locks_held);

//type definition for each lockset
typedef unsigned long LOCKS_CANDIDATE, LOCKS_HELD, WRITE_LOCKS_HELD;

//type definition for boolean type (obsolete in advanced c++ version)
typedef enum {FALSE, TRUE} bool;

//type definition for each state of each shared variable
typedef enum {VIRGIN, EXCLUSIVE, SHARED, SHARED_MODIFIED} STATE;

//global flag whether data race is detected
bool data_race_detected;

//type definition for each shared variable struct
typedef struct {
  int *data;    //memory location of each shared variable
  LOCKS_CANDIDATE locks_candidate;  // of each shared variable
  pthread_t thread_id;  //first thread that access each shared variable
  STATE state;    //state of each shared variable
}DATA;

//type definition for each thread
typedef struct {
  pthread_t thread; //pthread_t object
  LOCKS_HELD locks_held;    //locks held by each thread
  WRITE_LOCKS_HELD write_locks_held;    //write locks held by each thread
}THREAD;

//maximum size for each array (obsolete in advanced c++ version, substituted by vector container
#define MAX 100

//array to store shared variable
DATA shared_var_array[MAX];

//index for the array of shared variables
int shared_var_index;

//array to store locks (memory location of each pthread_mutex_t object)
void *mutex_array[MAX];

//index for the array of locks
int mutex_index;

//array to store information of all the threads
THREAD thread_array[MAX];

//index of thread array
int thread_index;

//lock for all newly added code
pthread_mutex_t this_thread;

//used in thread routine function
void my_thread_start() {
  pthread_mutex_lock(&this_thread);
  DBG_PRINT;    //print out debug information
  
  //initialize a thread and store it in the array
  thread_array[thread_index].thread = pthread_self();
  thread_array[thread_index].locks_held = 0;
  thread_array[thread_index].write_locks_held = 0;
  ++thread_index;

  pthread_mutex_unlock(&this_thread);
}

//used in thread routine function
void my_thread_end() {
  pthread_mutex_lock(&this_thread);
  DBG_PRINT;    //print out debug information
  pthread_mutex_unlock(&this_thread);
}

//used in main function
void my_main_start() {
  DBG_PRINT;    //print out debug information
  data_race_detected = FALSE;   //initialize the global flag
  
  //initialize all the indexes for each array
  shared_var_index = 0;

  mutex_index = 0;

  thread_index = 0;
  pthread_mutex_init(&this_thread, NULL);   //initialize the lock for newly added code
}

//used in main function
void my_main_end()  {
  DBG_PRINT;    //print out debug information
  if (data_race_detected == TRUE)   //if data race is detected, print out message
    printf("Warning: data race detected!\n");
  else printf("data race not detected!\n");
}

//used in main function
void my_obj_reg(int *data)  {
  DBG_PRINT;    //print out debug information

  //initialize a shared variable and store it in the array
  shared_var_array[shared_var_index].data = data;
  shared_var_array[shared_var_index].locks_candidate = 0;
  shared_var_array[shared_var_index].thread_id = 0;
  shared_var_array[shared_var_index].state = VIRGIN;

  ++shared_var_index;
}

//used in thread routine function
void my_obj_read(int *data) {
  pthread_mutex_lock(&this_thread);
  DBG_PRINT;    //print out debug information

  int tmp_var_index, tmp_thread_index;
  tmp_var_index = my_find_var(data);    //find specified shared variable and return its index in the array
  tmp_thread_index = my_find_thread(pthread_self());    //find specified thread and return its index in the array

  if (tmp_var_index != -1 && tmp_thread_index != -1) {  //if found
    switch (shared_var_array[tmp_var_index].state) { //maintain each lockset based on the state of specified shared variable and thread

    case VIRGIN:  //record the first thread accessed the shared variable if it has never been accessed
      shared_var_array[tmp_var_index].thread_id = pthread_self();
      PRINT   //print out the information of current shared variable
    break;
          
    case EXCLUSIVE:
      if (thread_array[tmp_thread_index].locks_held != 0) //refine the lock candidates of current accessed shared variable if current thread is holding some lock
        shared_var_array[tmp_var_index].locks_candidate = thread_array[tmp_thread_index].locks_held;
      PRINT   //print out the information of current shared variable
      //update current shared variable's state if current thread is not the first thread that accessed this shared variable
      if (shared_var_array[tmp_var_index].thread_id != pthread_self())
        shared_var_array[tmp_var_index].state = SHARED;
      else break;

    case SHARED:
      if (thread_array[tmp_thread_index].locks_held != 0) //refine the lock candidates of current accessed shared variable if current thread is holding some lock
        shared_var_array[tmp_var_index].locks_candidate &= thread_array[tmp_thread_index].locks_held;
      PRINT   //print out the information of current shared variable
      break;

    case SHARED_MODIFIED:
      if (thread_array[tmp_thread_index].locks_held != 0) //refine the lock candidates of current accessed shared variable if current thread is holding some lock
        shared_var_array[tmp_var_index].locks_candidate &= thread_array[tmp_thread_index].locks_held;
      PRINT   //print out the information of current shared variable
      if(shared_var_array[tmp_var_index].locks_candidate == 0)    //update the global flag if data race is detected
        data_race_detected = TRUE;

    default:
      PRINT   //print out the information of current shared variable
      break;
    }
  }
  pthread_mutex_unlock(&this_thread);
}

//used in thread routine function
void my_obj_write(int *data) {
  pthread_mutex_lock(&this_thread);
  DBG_PRINT;    //print out debug information

  int tmp_var_index, tmp_thread_index;
  tmp_thread_index = my_find_thread(pthread_self());    //find specified thread and return its index in the array
  if (tmp_thread_index != -1)   //if found
    thread_array[tmp_thread_index].write_locks_held = thread_array[tmp_thread_index].locks_held;  //update write locks held by current thread

  tmp_var_index = my_find_var(data);    //find specified shared variable and return its index in the array

  if (tmp_var_index != -1 && tmp_thread_index != -1) {  //if found
    switch (shared_var_array[tmp_var_index].state) { //maintain each lockset based on the state of specified shared variable and thread

    case VIRGIN:
      shared_var_array[tmp_var_index].state = EXCLUSIVE; //update current shared variable's state if it has never been accessed
      shared_var_array[tmp_var_index].thread_id = pthread_self(); //record the first thread accessed the shared variableif it has never been accessed
      //refine the lock candidates of current accessed shared variable if current thread is holding some lock and current shared variable is not consistently protected by any lock candidate
      if (shared_var_array[tmp_var_index].locks_candidate == 0 && thread_array[tmp_thread_index].locks_held != 0)
        shared_var_array[tmp_var_index].locks_candidate = thread_array[tmp_thread_index].locks_held;
      PRINT   //print out the information of current shared variable
      break;

    case EXCLUSIVE:
      //refine the lock candidates of current accessed shared variable if current thread is holding some lock and current shared variable is not consistently protected by any lock candidate
      if (shared_var_array[tmp_var_index].locks_candidate == 0 && thread_array[tmp_thread_index].locks_held != 0)
        shared_var_array[tmp_var_index].locks_candidate = thread_array[tmp_thread_index].locks_held;
      PRINT   //print out the information of current shared variable
      //update current shared variable's state if current thread is not the first thread that accessed this shared variable
      if (shared_var_array[tmp_var_index].thread_id != pthread_self())
        shared_var_array[tmp_var_index].state = SHARED_MODIFIED;
      //refine the lock candidates of current accessed shared variable if current thread is holding some write lock
      if (thread_array[tmp_thread_index].write_locks_held != 0)
        shared_var_array[tmp_var_index].locks_candidate &= thread_array[tmp_thread_index].write_locks_held;
      if (shared_var_array[tmp_var_index].locks_candidate == 0)   //update the global flag if data race is detected
        data_race_detected = TRUE;
      break;

    case SHARED:
      shared_var_array[tmp_var_index].state = SHARED_MODIFIED;   //update current shared variable's state

    case SHARED_MODIFIED:
      //refine the lock candidates of current accessed shared variable if current thread is holding some write lock
      if (thread_array[tmp_thread_index].write_locks_held != 0)
        shared_var_array[tmp_var_index].locks_candidate &= thread_array[tmp_thread_index].write_locks_held;
      PRINT   //print out the information of current shared variable
      if (shared_var_array[tmp_var_index].locks_candidate == 0)   //update the global flag if data race is detected
        data_race_detected = TRUE;

    default:
      PRINT   //print out the information of current shared variable
      break;
    }
  }
  pthread_mutex_unlock(&this_thread);
}

//used in main function
int my_mutex_init(pthread_mutex_t *mutex, const pthread_mutexattr_t *attr) {
  DBG_PRINT;    //print out debug information
  mutex_array[mutex_index++] = mutex;   //store the lock (pthread_mutex_t object) in the array
  pthread_mutex_init(mutex, attr);  //initialize the lock (pthread_mutex_t object)
}

//used in main function
int my_mutex_destroy(pthread_mutex_t *mutex) {
  DBG_PRINT;    //print out debug information
  pthread_mutex_destroy(mutex); //destroy used lock (pthread_mutex_t object)
}

//used in thread routine function
int my_mutex_lock(pthread_mutex_t *mutex) {
  pthread_mutex_lock(mutex);    //call pthread_mutex_lock

  pthread_mutex_lock(&this_thread);
  DBG_PRINT;    //print out debug information

  int tmp_mutex_index, tmp_thread_index;
  tmp_mutex_index = my_find_mutex(mutex);   //find specified lock (pthread_mutex_t object) and return its index in the array
  tmp_thread_index = my_find_thread(pthread_self());    //find specified thread and return its index in the array

  if (tmp_mutex_index != -1 && tmp_thread_index != -1)  //if found
    thread_array[tmp_thread_index].locks_held |= 1<<tmp_mutex_index;    //update locks held by current thread
  PRINT1    //print out the information of all shared variables

  pthread_mutex_unlock(&this_thread);
}
int my_mutex_unlock(pthread_mutex_t *mutex) {

  pthread_mutex_lock(&this_thread);
  DBG_PRINT;    //print out debug information

  int i;

  int tmp_mutex_index, tmp_thread_index;
  tmp_mutex_index = my_find_mutex(mutex);   //find specified lock (pthread_mutex_t object) and return its index in the array
  tmp_thread_index = my_find_thread(pthread_self());    //find specified thread and return its index in the array

  if (tmp_mutex_index != -1 && tmp_thread_index != -1) {    //if found
    thread_array[tmp_thread_index].locks_held &= ~(1<<tmp_mutex_index); //update locks held by current thread
    thread_array[tmp_thread_index].write_locks_held &= ~(1<<tmp_mutex_index);   //update write locks held by current thread
  }

  PRINT1    //print out the information of all shared variables
  pthread_mutex_unlock(&this_thread);

  pthread_mutex_unlock(mutex);
}
int my_thread_create(pthread_t *thread, const pthread_attr_t * attr, void*(*start_routine)(void*), void *arg) {
  DBG_PRINT;    //print out debug information
  pthread_create(thread, attr, start_routine, arg); //create thread
}
int my_thread_join(pthread_t thread, void **value_ptr) {
  DBG_PRINT;    //print out debug information
  pthread_join(thread, value_ptr);  //join thread
}

//find specified lock (pthread_mutex_t object)
int my_find_mutex(pthread_mutex_t *mutex) {
  int i, tmp_mutex_index;

  tmp_mutex_index = mutex_index;
  for (i = 0; i<tmp_mutex_index; ++i)
    if (mutex_array[i] == mutex)
      return i; //return its index in the array if found

  return -1;    //return -1 if failed
}

//find specified thread
int my_find_thread(pthread_t thread) {
  int i, tmp_thread_index;

  tmp_thread_index = thread_index;

  for (i = 0; i<tmp_thread_index; ++i)

    if (thread_array[i].thread == thread)
      return i; //return tis index in the array if found

  return -1;    //return -1 if failed

}

//find specified shared variable
int my_find_var(int *data) {
  int i, tmp_var_index;
  tmp_var_index = shared_var_index;
  for (i = 0; i<shared_var_index; ++i)
    if (shared_var_array[i].data == data)
      return i; //return its index in the array if found
  return -1;    //return -1 if failed
}
