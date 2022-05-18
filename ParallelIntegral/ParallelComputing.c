#define _GNU_SOURCE
#include <pthread.h>
#include <sched.h>
#include <sys/sysinfo.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <error.h>

#include "Integral.h"

#define DATA_SIZE sizeof(double) * 2 + sizeof(int) 
#define PAGE_SIZE 4096


#define HANDLE_ERROR(str)    do{             \
                                perror(str);\
                                exit(0);    \
                            } while (0)       

struct ThreadData
{
    double from;
    double to;
    int thread_num;
    double result[(PAGE_SIZE - DATA_SIZE) / sizeof(double)];
};


void ThreadSet(int num_of_threads, pthread_t* thr_info, struct ThreadData* data);
void* ThreadBody(void* args);
double IntegralCompute(int num_of_threads, pthread_t* thr_info, struct ThreadData* data);
void ThreadJoin(int num_of_threads, pthread_t* thr_info);
int ThreadMounting(int num_of_threads, pthread_t** thr_info, int num_of_real_threads);



int main (int argc, char** argv) 
{
    if (argc < 2) {

        printf("Not enough parameters\n");
        return 0;
    }

    int num_of_threads = atoi(argv[1]);

    if (num_of_threads < 1) {

        printf("Not enough threads\n");
        return 0;
    }

    pthread_t* thr_info = (pthread_t*) calloc (num_of_threads, sizeof(pthread_t));
    if (thr_info == NULL) HANDLE_ERROR("alloc threads:");

    struct ThreadData* data = (struct ThreadData*) calloc (num_of_threads, sizeof(struct ThreadData));
    if (data == NULL) HANDLE_ERROR("alloc data:");

    ThreadSet (num_of_threads, thr_info, data);
    printf ("The result is: %lf", IntegralCompute (num_of_threads, thr_info, data));

    free(thr_info);
    free(data);
}


void* ThreadBody(void* args) 
{

    struct ThreadData* data = (struct ThreadData*)args;
    cpu_set_t cpuset;

    CPU_ZERO(&cpuset);
    CPU_SET(data->thread_num, &cpuset);

    pthread_setaffinity_np(pthread_self(), sizeof(cpuset), &cpuset);

    *data->result = Integralling (data->from, data->to);

    return NULL;
}


void ThreadJoin (int num_of_threads, pthread_t* thr_info) 
{
    for (int i = 0; i < num_of_threads; i++) {

        if (pthread_join(thr_info[i], NULL) != 0) HANDLE_ERROR("thread join:");
    }
}


void ThreadSet (int num_of_threads, pthread_t* thr_info, struct ThreadData* data) 
{
    int procs_num = get_nprocs();
    pthread_t* im_theads = NULL;

    int im_threads_num = ThreadMounting (procs_num - num_of_threads, &im_theads, num_of_threads);

    for (int i = 0; i < num_of_threads; i++) {

        data[i].from = i * (RANGE / num_of_threads);
        data[i].to = data[i].from + (RANGE / num_of_threads);
        data[i].thread_num = (im_threads_num + i) % procs_num;

        if (pthread_create(thr_info + i, NULL, ThreadBody, data + i) != 0) HANDLE_ERROR("thread create:");
    }

    ThreadJoin(im_threads_num, im_theads);
}

int ThreadMounting(int num_of_threads, pthread_t** thr_info, int num_of_real_threads)
{

    if (num_of_threads <= 0) return 0;

    double adj_coef = 1;
    *thr_info = (pthread_t*)calloc(num_of_threads, sizeof(pthread_t));
    if (thr_info == NULL) HANDLE_ERROR("alloc threads:");

    struct ThreadData* data = (struct ThreadData*)calloc(num_of_threads, sizeof(struct ThreadData));
    if (data == NULL) HANDLE_ERROR("alloc data:");

    for (int i = 0; i < num_of_threads; i++) {

        data[i].from = 0;
        data[i].to = (RANGE / num_of_real_threads) * adj_coef;
        data[i].thread_num = i;

        if (pthread_create((*thr_info) + i, NULL, ThreadBody, data + i) != 0) HANDLE_ERROR("thread create:");
    }

    return num_of_threads;
}


double IntegralCompute(int num_of_threads, pthread_t* thr_info, struct ThreadData* data)
{

    double integral = 0;

    for (int i = 0; i < num_of_threads; i++) {

        if (pthread_join(thr_info[i], NULL) != 0) HANDLE_ERROR("thread join:");

        integral += data[i].result[0];
    }

    return integral;
}