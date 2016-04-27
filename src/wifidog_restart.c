#include <stdio.h>
#include <stdlib.h>
#include <signal.h>

#include "pid.h"

int main()
{
	pid_t pid;
	if (find_pid_by_name("wifidog", &pid, 1) > 0) {
		kill(pid, SIGTERM);
        while (kill(pid, 0) != -1) {
            sleep(1);
		}
		system("wifidog");
	}
	return 0;
}
