//test_case_03.c
//written by Chang Liu
//three threads access a shared variable
//two threads writes content to data
//while another thread reads content from data
//test whether my data race detector can work well
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

  my_thread_start();

  //my_mutex_lock(&A);
  //printf("thread 1 : lock (A)\n");
  my_obj_read(&data);
  tmp = data;
  //my_mutex_unlock(&A);
  //my_mutex_lock(&B);  
  my_obj_write(&data);
  data = tmp+1;
  
  //my_mutex_unlock(&B);
  //printf("thread 1 : unlock (A)\n");

  my_thread_end();

  return 0;
}

void * thread_routine_2(void * arg)
{
  int tmp;

  my_thread_start();
  //my_mutex_lock(&B);
  //printf("thread 2 : lock (B)\n");
  
  my_obj_read(&data);
  tmp = data;
  
  //my_mutex_unlock(&B);
  //printf("thread 2 : unlock (B)\n");
  my_thread_end();

  return 0;
}

void * thread_routine_3(void * arg)
{
  int tmp;

  my_thread_start();

  //my_mutex_lock(&C);
  //printf("thread 3 : lock (C)\n");
                                                                                                                                                                      
  my_obj_write(&data);
  data = tmp+1;
                                                                                                                                                                   
  //my_mutex_unlock(&C);
  //printf("thread 3 : unlock (C)\n");

  my_thread_end();

  return 0;
}

int main()
{
  pthread_t  t1, t2, t3;

  my_main_start();

  my_obj_reg(&data);

  my_mutex_init(&A, NULL);
  my_mutex_init(&B, NULL);
  my_mutex_init(&C, NULL);

  my_thread_create(&t1, 0, thread_routine_1, 0);
  my_thread_create(&t2, 0, thread_routine_2, 0);
  my_thread_create(&t3, 0, thread_routine_3, 0);

  my_thread_join(t1, 0);
  my_thread_join(t2, 0);
  my_thread_join(t3, 0);

  my_mutex_destroy(&A);
  my_mutex_destroy(&B);
  my_mutex_destroy(&C);

  my_main_end();

  return 0;
}
