#ifndef _CLIENT_HASH_H_
#define _CLIENT_HASH_H_

#include <time.h>
#include "uthash.h"

/* for provisional release */
struct release_client {
	char ip[16];
    time_t last_checkin;
	ut_hash_handle hh;						/**< @brief Make this struct hashable */
};

struct release_client *release_client_find(const char *ip);
void release_client_add(const char *ip);
void release_client_del(struct release_client *client);
void release_client_timeout(void);

#endif /* _CLIENT_HASH_H_ */
