#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "pid.h"

#define CHECK_INTERVAL	3	/* seconds */

int main()
{
	pid_t pid;
	for (;;) {
		if (!find_pid_by_name("wifidog", &pid, 1))
			system("wifidog");
		sleep(CHECK_INTERVAL);
	}
	return 0;
}
