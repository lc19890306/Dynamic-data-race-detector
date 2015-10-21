//test_case_03.o.c
//written by Chang Liu
//three threads access a shared variable
//two threads writes content to data
//while another thread reads content from data
//controlled test
//whether data race can be detected
//if no lock consistently protect all the shared variables

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

int data;

pthread_mutex_t A;
pthread_mutex_t B;
pthread_mutex_t C;

void * thread_routine_1(void * arg)
{
  int tmp;

  //pthread_mutex_lock(&A);
  //printf("thread 1 : lock (A)\n");

  tmp = data;

  data = tmp+1;
  
  //pthread_mutex_unlock(&A);
  //printf("thread 1 : unlock (A)\n");

  return 0;
}

void * thread_routine_2(void * arg)
{
  int tmp;

  //pthread_mutex_lock(&B);
  //printf("thread 2 : lock (B)\n");
  
  tmp = data;
  
  //pthread_mutex_unlock(&B);
  //printf("thread 2 : unlock (B)\n");

  return 0;
}

void * thread_routine_3(void * arg)
{
  int tmp;

  //pthread_mutex_lock(&C);
  //printf("thread 3 : lock (C)\n");
  
  data = tmp+1;
                                                                                                                                                                    
  //pthread_mutex_unlock(&C);
  //printf("thread 3 : unlock (C)\n");

  return 0;
}

int main()
{
  pthread_t  t1, t2, t3;

  pthread_mutex_init(&A, NULL);
  pthread_mutex_init(&B, NULL);
  pthread_mutex_init(&C, NULL);

  pthread_create(&t1, 0, thread_routine_1, 0);
  pthread_create(&t2, 0, thread_routine_2, 0);
  pthread_create(&t3, 0, thread_routine_3, 0);

  pthread_join(t1, 0);
  pthread_join(t2, 0);
  pthread_join(t3, 0);

  pthread_mutex_destroy(&A);
  pthread_mutex_destroy(&B);
  pthread_mutex_destroy(&C);

  return 0;
}
