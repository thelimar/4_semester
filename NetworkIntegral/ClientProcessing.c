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

#include "NetworkHeader.h"

#define e 2.71828182846
#define PAGE_SIZE 4096

int num_proc;
int per_core;

int max_int(int first, int second) {
	if (first > second) return first;
	else return second;
}

struct integral_param 
{
	pthread_t thread_id;
	double start;
	double fin;
	double delta;
	int num;
	double sum_local[PAGE_SIZE / sizeof(double)];
};

int read_number(const char* str, int* num) 
{
	if (str == NULL) return -1;

	const char* iter = str;
	while (!isdigit(*iter)) iter++;
	return sscanf(iter, "%d", num);
}

int get_per_core() 
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

void* integral(void* data) 
{
	struct integral_param* param = data;

	cpu_set_t cpuset;
	CPU_ZERO(&cpuset);

	int num_real_cpu = num_proc / per_core;
	int num_of_cpu = ((param->num % num_real_cpu) * per_core + param->num / num_real_cpu) % num_proc;

	CPU_SET(num_of_cpu, &cpuset);

	int err = pthread_setaffinity_np(param->thread_id, sizeof(cpuset), &cpuset);
	if (err != 0) {
		printf("Err is %d in %d\n", err, param->num);
		perror("CPU_ALLOC");
	}

	param->sum_local[0] = 0;

	for (; param->start < param->fin; param->start += param->delta)
	{
		double x = param->start + param->delta / 2;
		param->sum_local[0] += (pow(e, x) + pow(e, -x)) * param->delta;
	}
}


double compute_integral(int num_threads, struct boards_info general_boards) 
{
	num_proc = get_nprocs_conf(); // get_nprocs();
	per_core = get_per_core();

	int threads = num_threads;

	if (threads < 1) {
		printf("minimal threads num - 1\n");
	}
	if (threads > num_proc) {
		threads = num_proc;
	}

	pthread_t thread_ids[num_proc];
	struct integral_param params[num_proc];

	double global_start = general_boards.left;
	double global_fin = general_boards.right;
	double global_delta = general_boards.delta;

	double interval = (global_fin - global_start) / threads;

	int err = 0;

	for (int i = 0; i < threads; i++) {

		params[i].start = global_start + i * interval;
		params[i].fin = global_start + (i + 1) * interval;
		params[i].delta = global_delta;
		params[i].num = i;
	}
	for (int i = threads; i < num_proc; i++) {

		params[i].start = global_start;
		params[i].fin = global_start + interval;
		params[i].delta = global_delta;
		params[i].num = i;
	}

	double sum_global = 0;

	for (int i = 0; i < num_proc; i++) {

		err = pthread_create(&(params[i].thread_id), NULL, integral, &params[i]);
	}
	for (int i = 0; i < num_proc; i++) {

		err = pthread_join(params[i].thread_id, NULL);
		if (i < threads) sum_global += params[i].sum_local[0];
	}

	return sum_global;
}