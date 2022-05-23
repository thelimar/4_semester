
#define _GNU_SOURCE
#include <math.h>
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>
#include <ctype.h>
#include <errno.h>
#include <sys/sysinfo.h>
#include <string.h>
#include "Integral.h"

#define PAGE_SIZE 4096

int num_proc;
int per_core;


struct ThreadData
{
	pthread_t thread_id;
	double start;
	double fin;
	double delta;
	int num;
	double sum_local[PAGE_SIZE / sizeof(double)];
};

void* IntegralCompute(void* data);
int max_int(int first, int second);
int min_int(int first, int second);
int ThreadMounting();


int main(int argc, char** argv)
{
	num_proc = get_nprocs_conf();
	per_core = ThreadMounting();

	if (argc < 2) 
	{
		fprintf (stderr, "Enter only 1 arg\r\n");
		exit (EXIT_FAILURE);
	}

	int threads = atoi(argv[1]);

	if (threads < 1) 
	{
		printf("Number of threads must be more than 0\r\n");
		return 1;
	}

	int max_thread  = max_int (threads, num_proc);
	int real_thread = min_int (threads, num_proc);

	pthread_t* thread_ids = (pthread_t*) calloc(max_thread, sizeof(pthread_t));

	if (thread_ids == NULL) 
	{
		printf("Can't alloc that much memory\n");
		perror("thread_ids");
		return 2;
	}

	struct ThreadData* params = (struct ThreadData*)calloc(max_thread, sizeof(struct ThreadData));

	if (params == NULL) 
	{
		printf("Memory allocation error\r\n");
		perror("params");
		return -1;
	}

	double global_start = -10;
	double global_fin = 10;
	double global_delta = 0.0000004;

	double interval = (global_fin - global_start) / real_thread;

	int err = 0;

	for (int i = 0; i < real_thread; i++) {

		params[i].start = global_start + i * interval;
		params[i].fin = global_start + (i + 1) * interval;
		params[i].delta = global_delta;
		params[i].num = i;
	}

	for (int i = threads; i < max_thread; i++) 
	{
		params[i].start = global_start;
		params[i].fin = global_start + interval;
		params[i].delta = global_delta;
		params[i].num = i;
	}

	double sum_global = 0;
	int created = 0;

	for (int i = 0; i < max_thread; i++) 
	{
		err = pthread_create(&(params[i].thread_id), NULL, IntegralCompute, &params[i]);
		if (err == 0) created++;
	}


	for (int i = 0; i < created; i++) 
	{
		err = pthread_join(params[i].thread_id, NULL);
		if (err != 0) {
			perror ("pthread_join");
		}

		if (i < real_thread) sum_global += params[i].sum_local[0];
	}

	printf ("Result is: %lf\r\n", sum_global);

	free(thread_ids);
	free(params);
}


int max_int(int first, int second) 
{
	if (first > second) return first;
	else return second;
}


int min_int(int first, int second) 
{
	if (first < second) return first;
	else return second;
}


int read_number(const char* str, int* num) 
{
	if (str == NULL) return -1;

	const char* iter = str;
	while (!isdigit(*iter)) iter++;
	return sscanf(iter, "%d", num);
}

int ThreadMounting()
{
	per_core = 2;

	FILE* cpu_info_file = popen("lscpu -y", "r");

	if (cpu_info_file != NULL) {

		char* file_buf = NULL;
		size_t dump_size = 0;

		getdelim(&file_buf, &dump_size, '\0', cpu_info_file);

		if (file_buf == NULL) return per_core;

		char* place = strstr(file_buf, "Thread(s) per core:");
		read_number(place, &per_core);

		free(file_buf);

	}
	else 
	{
		FILE* file_ht_active = fopen("/sys/devices/system/cpu/smt/active", "r");
		if (file_ht_active == NULL) return per_core;

		int active = 0;
		int err = fscanf(file_ht_active, "%d", &active);
		if (err < 1) return per_core;

		if (active > 0) per_core = 2; // HT active
		else per_core = 1;

		fclose(file_ht_active);
	}

	fclose(cpu_info_file);

	return per_core;
}


void* IntegralCompute (void* data) 
{
	struct ThreadData* param = data;

	cpu_set_t cpuset;
	CPU_ZERO(&cpuset);

	int num_real_cpu = num_proc / per_core;
	int num_of_cpu = ((param->num % num_real_cpu) * per_core + param->num / num_real_cpu) % num_proc;

	CPU_SET(num_of_cpu, &cpuset);

	int err = pthread_setaffinity_np(param->thread_id, sizeof(cpuset), &cpuset);

	if (err != 0) 
	{
		printf("Err is %d in %d\n", err, param->num);
		perror("CPU_ALLOC");
	}

	param->sum_local[0] = 0;

	for (; param->start < param->fin; param->start += param->delta) 
	{
		double x = param->start + param->delta / 2;
		param->sum_local[0] += func (x) * param->delta;
	}

	return NULL;
}