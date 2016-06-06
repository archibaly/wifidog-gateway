#include <string.h>

#include "str.h"

char *ltrim(char *s)
{
	char *p = s;
	while (ISSPACE(*p))
		p++;
	strcpy(s, p);
	return s;
}

char *rtrim(char *s)
{
	int i;

	i = strlen(s) - 1;
	while (ISSPACE(s[i]))
		i--;
	s[i + 1] = '\0';

	return s;
}
 
char *trim(char *s)
{
	ltrim(s);
	rtrim(s);
	return s;
}

int streq(const char *a, const char *b)
{
	return strcmp(a, b) == 0 ? 1 : 0;
}

int is_lower(char ch)
{
	if (ch >= 'a' && ch <= 'z')
		return 1;
	return 0;
}

int is_upper(char ch)
{
	if (ch >= 'A' && ch <= 'Z')
		return 1;
	return 0;
}

char *strlower(char *str)
{
	int i;
	int len = strlen(str);
	for (i = 0; i < len; i++)
		if (is_upper(str[i]))
			str[i] += 32;
	str[len] = '\0';
	return str;
}

char *strupper(char *str)
{
	int i;
	int len = strlen(str);
	for (i = 0; i < len; i++)
		if (is_lower(str[i]))
			str[i] -= 32;
	str[len] = '\0';
	return str;
}

int find_char(const char *str, char ch)
{
    int i;
    for (i = 0; str[i] != '\0'; i++) {
        if (str[i] == ch)
            return i;
    }
    return -1;
}
