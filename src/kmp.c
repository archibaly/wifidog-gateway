#include <string.h>

#include "kmp.h"

static void get_next(const char *p, int *next)
{
	int i = 0, j = -1;
	int len = strlen(p);
	next[0] = -1;

	while (i < len) {
		if (j == -1 || p[i] == p[j]) {
			i++; j++;
			if (p[i] != p[j])
 				next[i] = j;
			else
				next[i] = next[j];
		} else {
				j = next[j];
		}
	}
}

static int get_pos(const char *t, const char *p, int *next)
{
	int i = 0, j = 0;
	int t_len = strlen(t);
	int p_len = strlen(p);

	while (i < t_len && j < p_len) {
		if (j == -1 || t[i] == p[j]) {
			i++; j++;
		} else {
			j = next[j];
		}
	}
	if (j == p_len)
		return i - p_len;
	else
		return -1;
}

/*
 * @t: target string
 * @p: pattern string
 */
int kmp(const char *t, const char *p)
{
	int next[strlen(p)];
	get_next(p, next);
	return get_pos(t, p, next);
}
