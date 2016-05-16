#ifndef _POPEN_H_
#define _POPEN_H_

#include <stdio.h>

FILE *popen(const char *cmdstring, const char *type);
int pclose(FILE *fp);

#endif /* _POPEN_H_ */
