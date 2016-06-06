#ifndef _STR_H_
#define _STR_H_

#define ISSPACE(x)	((x)==' '||(x)=='\t'||(x)=='\n')

char *ltrim(char *s);
char *rtrim(char *s);
char *trim(char *str);
int streq(const char *a, const char *b);
int is_lower(char ch);
int is_upper(char ch);
char *strlower(char *str);
char *strupper(char *str);
int find_char(const char *str, char ch);

#endif /* _STR_H_ */
