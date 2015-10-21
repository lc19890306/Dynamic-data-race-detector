//test_case_01.c
//written by Chang Liu
//two threads access two shared variables
//a thread writes content to data2
//while the other thread reads content from data2
//data race reported in strict lockset algorithm
//but not reported in improved version
//test whether data race will not be reported
//in read-shared case and reader-writer case

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

  my_thread_start();

  my_mutex_lock(&A);
  printf("thread 1 : lock (A)\n");
  my_obj_read(&data1);
  tmp = data1;
  
  my_obj_write(&data2);
  data2 = tmp+1;
  
  my_mutex_unlock(&A);
  printf("thread 1 : unlock (A)\n");

  my_thread_end();

  return 0;
}

void * thread_routine_2(void * arg)
{
  int tmp;

  my_thread_start();
  my_mutex_lock(&B);
  printf("thread 2 : lock (B)\n");
  my_obj_read(&data2);
  tmp = data2;
  
  my_mutex_lock(&A);
  printf("thread 2 : lock (A)\n");
  my_obj_write(&data1);
  data1 = tmp-1;
  
  my_mutex_unlock(&A);
  printf("thread 2 : unlock (A)\n");
  my_mutex_unlock(&B);
  printf("thread 2 : unlock (B)\n");
  my_thread_end();

  return 0;
}


int main()
{
  pthread_t  t1, t2;

  my_main_start();

  my_obj_reg(&data1);
  my_obj_reg(&data2);

  my_mutex_init(&A, NULL);
  my_mutex_init(&B, NULL);

  my_thread_create(&t1, 0, thread_routine_1, 0);
  my_thread_create(&t2, 0, thread_routine_2, 0);

  my_thread_join(t1, 0);
  my_thread_join(t2, 0);

  my_mutex_destroy(&A);
  my_mutex_destroy(&B);

  my_main_end();

  return 0;
}
