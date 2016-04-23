#ifndef _CLIENT_HASH_H_
#define _CLIENT_HASH_H_

#include <time.h>
#include "uthash.h"

/* Counters struct for a client's bandwidth usage (in bytes) */
typedef struct _t_counters {
	unsigned long long incoming;			/**< @brief Incoming data total*/
	unsigned long long outgoing;			/**< @brief Outgoing data total*/
	unsigned long long incoming_history;	/**< @brief Incoming data before wifidog restarted*/
	unsigned long long outgoing_history;	/**< @brief Outgoing data before wifidog restarted*/
	/* Delta traffic stats by t123yh */
	unsigned long long incoming_delta;		/**< @brief Incoming data after last report*/
	unsigned long long outgoing_delta;		/**< @brief Outgoing data after last report*/
	time_t last_updated;					/**< @brief Last update of the counters */
} t_counters;

/* Client node for the connected client */
typedef struct _t_client {
	char *ip;								/**< @brief Client Ip address */
	char *mac;								/**< @brief Client Mac address */
	char *token;							/**< @brief Client token */
	int fw_connection_state;				/**< @brief Connection state in the firewall */
	int fd;									/**< @brief Client HTTP socket (valid only during login before one of the _http_* function is called */
	t_counters counters;					/**< @brief Counters for input/output of the client. */
	ut_hash_handle hh;						/**< @brief Make this struct hashable */
} t_client;

int client_add(t_client *client);

#endif /* _CLIENT_HASH_H_ */
