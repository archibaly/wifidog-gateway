#include <syslog.h>
#include "safe.h"
#include "debug.h"
#include "auth.h"
#include "common.h"
#include "client_hash.h"
#include "fw_iptables.h"
#include "centralserver.h"

static struct release_client *head = NULL;

struct release_client *release_client_find(const char *ip)
{
	struct release_client *client;
	HASH_FIND_STR(head, ip, client);
	return client;
}

void release_client_add(const char *ip)
{
	debug(LOG_INFO, "RELEASE CLIENT ADD");
	struct release_client *client;
	HASH_FIND_STR(head, ip, client);
	if (!client) {
		client = safe_malloc(sizeof(struct release_client));
		strncpy(client->ip, ip, 15);
		client->last_checkin = time(NULL);
		HASH_ADD_STR(head, ip, client);
        iptables_do_command("-t mangle -A " CHAIN_TRUSTED " -s %s -j MARK --set-mark %d", ip, FW_MARK_KNOWN);
	}
}

void release_client_del(struct release_client *client)
{
	debug(LOG_INFO, "RELEASE CLIENT DEL");
	iptables_do_command("-t mangle -D " CHAIN_TRUSTED " -s %s -j MARK --set-mark %d", client->ip, FW_MARK_KNOWN);
	HASH_DEL(head, client);
	free(client);
}

void release_client_timeout(void)
{
	struct release_client *cur;
	struct release_client *tmp;
	t_client *client;
	t_authresponse authresponse;

	HASH_ITER(hh, head, cur, tmp) {
		if (cur == NULL)
			return;
		if (time(NULL) - cur->last_checkin >= RELEASE_TIMEOUT) {
			/* send login request with a determined token */
			LOCK_CLIENT_LIST();
			client = client_list_find_by_ip(cur->ip);
			if (client) {
				debug(LOG_INFO, "FOUND CLIENT");
				auth_server_request(&authresponse, REQUEST_TYPE_LOGIN, client->ip, client->mac, "83485623059023", 0, 0, 0, 0);
				if (authresponse.authcode == AUTH_ALLOWED) {
					fw_allow(client, FW_MARK_KNOWN);
				}
			}
			UNLOCK_CLIENT_LIST();
			release_client_del(cur);
		}
	}
}
