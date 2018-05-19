#ifndef _SYSTEM_INFO_
#define _SYSTEM_INFO_


typedef struct sys_mem_s sys_mem_t;

struct sys_mem_s {

	long uptime;

	unsigned long loads[3];

	unsigned long total_ram;
	unsigned long free_ram;
	unsigned long shared_ram;
	unsigned long buffer_ram;

	unsigned long total_swap;
	unsigned long free_swap;

	unsigned short procs;

	unsigned long total_high;
	unsigned long free_high;

	unsigned int  mem_unit;
};


typedef struct sys_cpu_s sys_cpu_t;

struct sys_cpu_s {
	long  cpu_core;
	long  cpu_conf;
	long  cpu_rate;
	char  cpu_PSN[64]; //Can not get it, at now
};


typedef struct sys_net_s sys_net_t;

struct sys_net_s {
	char mac[64];
	char local_ipv4[64];
	char local_ipv6[64];
	char extern_ipv4[64];
	char extern_ipv6[64];
};


typedef struct sys_info_s sys_info_t;

struct sys_info_s {
	sys_cpu_t cpu;
	sys_mem_t mem;
};

sys_info_t *new_sys_info();
int demo(sys_info_t **sys_info);
int sys_info_start(sys_info_t *sys_info);

#endif
