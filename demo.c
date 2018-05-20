#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "system_info.h"

int main(int argc, char *argv[])
{
	sys_info_t sys_info;
	sys_demo(&sys_info);

	for (; ; )
	{
		sleep(1);
		printf("======");
		printf("cpu core: %ld\t total_ram: %ld\t free_ram: %ld\t mem_unit: %d\n", sys_info.cpu.cpu_core, sys_info.mem.total_ram, sys_info.mem.free_ram, sys_info.mem.mem_unit);
		printf("======");
	}
}
