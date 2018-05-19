#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <memory.h>
#include <signal.h>
#include <pthread.h>
#include <sys/sysinfo.h>

#include "system_info.h"

static void *sys_info_run(void *arg);

sys_info_t *new_sys_info()
{
	sys_info_t *sys_info = NULL;

	sys_info = (sys_info_t *) malloc(sizeof(*sys_info));
	if (sys_info == NULL)
	{
		return NULL;
	}

	memset(sys_info, 0x00, sizeof(*sys_info));

	return sys_info;
}

static void get_mem_info(sys_mem_t *sys_mem)
{
	int i = 0;
	struct sysinfo si;

	sysinfo(&si);

	for ( i = 0; i < 3; i++)
	{
		sys_mem->loads[i] = si.loads[i];
	}

	sys_mem->uptime     = si.uptime;
	sys_mem->free_ram   = si.freeram;
	sys_mem->total_ram  = si.totalram;
	sys_mem->shared_ram = si.sharedram;
	sys_mem->buffer_ram = si.bufferram;
	sys_mem->free_swap  = si.freeswap;
	sys_mem->total_swap = si.totalswap;
	sys_mem->procs      = si.procs;
	sys_mem->free_high  = si.freehigh;
	sys_mem->total_high = si.totalhigh;
	sys_mem->mem_unit   = si.mem_unit;
}

static void get_cpu_info(sys_cpu_t *sys_cpu)
{
	sys_cpu->cpu_core = sysconf(_SC_NPROCESSORS_ONLN);
	sys_cpu->cpu_conf = sysconf(_SC_NPROCESSORS_CONF);
	sys_cpu->cpu_rate = 0;
}


int sys_info_start(sys_info_t *sys_info)
{

	int       err;
	sigset_t  mask;
	sigset_t  oldmask;
	pthread_t ntid;

	sigemptyset(&mask);
	sigfillset(&mask);
	sigdelset(&mask, SIGINT);

	if (pthread_sigmask(SIG_BLOCK, &mask, &oldmask) < 0)
	{
		printf("pthread sigmask set error\n");
	}

	err = pthread_create(&ntid, NULL, sys_info_run, (void *)sys_info);

	if (pthread_sigmask(SIG_BLOCK, &oldmask, NULL) < 0)
	{
		printf("pthread sigmask set error\n");
	}

	if (err != 0)
	{
		printf("pthread_create error\n");
		return -1;
	}

	return 0;
}

static void *sys_info_run(void *arg)
{
	sys_info_t *sys_info = (sys_info_t *)arg;

	for ( ; ; )
	{
		get_mem_info(&sys_info->mem);
		get_cpu_info(&sys_info->cpu);
		sleep(1);
	}
}



int demo(sys_info_t **sys_info)
{
	*sys_info = new_sys_info();
	if (*sys_info == NULL)
	{
		return -1;
	}

	sys_info_start(*sys_info);
	return 0;
}











