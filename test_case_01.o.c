//test_case_01.o.c
//written by Chang Liu
//two threads access two shared variables
//a thread writes content to data2
//while another thread reads content from data2
//data race reported in strict lockset algorithm
//but not reported in improved version
//controlled test
//whether data race will and will not be reported
//in reader-writer case and read-shared case respectively

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

int data1;
int data2;

pthread_mutex_t A;
pthread_mutex_t B;

void * thread_routine_1(void * arg)
{
  int tmp;

  pthread_mutex_lock(&A);
  printf("thread 1 : lock (A)\n");

  tmp = data1;

  data2 = tmp+1;
  
  pthread_mutex_unlock(&A);
  printf("thread 1 : unlock (A)\n");

  return 0;
}

void * thread_routine_2(void * arg)
{
  int tmp;

  pthread_mutex_lock(&B);
  printf("thread 2 : lock (B)\n");

  tmp = data2;
  
  pthread_mutex_lock(&A);
  printf("thread 2 : lock (A)\n");

  data1 = tmp-1;
  
  pthread_mutex_unlock(&A);
  printf("thread 2 : unlock (A)\n");
  pthread_mutex_unlock(&B);
  printf("thread 2 : unlock (B)\n");

  return 0;
}


int main()
{
  pthread_t  t1, t2;

  pthread_mutex_init(&A, NULL);
  pthread_mutex_init(&B, NULL);

  pthread_create(&t1, 0, thread_routine_1, 0);
  pthread_create(&t2, 0, thread_routine_2, 0);

  pthread_join(t1, 0);
  pthread_join(t2, 0);

  pthread_mutex_destroy(&A);
  pthread_mutex_destroy(&B);

  return 0;
}
