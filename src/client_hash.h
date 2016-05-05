#ifndef _CLIENT_HASH_H_
#define _CLIENT_HASH_H_

#include <time.h>
#include "uthash.h"

/* for provisional release */
struct release_client {
	char *ip;
	char *mac;
	char *token;
	int timeout;			/**< @brief Release timeout */
    time_t time;			/**< @brief Checkin time */
	ut_hash_handle hh;		/**< @brief Make this struct hashable */
};

struct release_client *release_client_find(const char *ip);
void release_client_add(const char *ip, const char *token, int timeout);
void release_client_del(struct release_client *client);
void release_client_timeout(void);

#endif /* _CLIENT_HASH_H_ */
