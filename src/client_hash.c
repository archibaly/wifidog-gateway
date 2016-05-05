#include "safe.h"
#include "debug.h"
#include "auth.h"
#include "common.h"
#include "fw_iptables.h"
#include "centralserver.h"

#include "client_hash.h"

static struct release_client *head = NULL;

struct release_client *release_client_find(const char *ip)
{
	struct release_client *client;
	HASH_FIND_STR(head, ip, client);
	return client;
}

static struct release_client *release_client_new(const char *ip, const char *token, int timeout)
{
	char *mac;
	struct release_client *client;

	if (!(mac = arp_get(ip)))
		return NULL;

	client = safe_malloc(sizeof(struct release_client));
	client->ip = safe_strdup(ip);
	client->mac = mac;
	client->token = safe_strdup(token);
	client->timeout = timeout;
	client->time = time(NULL);

	return client;
}

void release_client_add(const char *ip, const char *token, int timeout)
{
	debug(LOG_INFO, "RELEASE CLIENT ADD");
	struct release_client *client;
	client = release_client_new(ip, token, timeout);
	if (client) {
		HASH_ADD_STR(head, ip, client);
		iptables_do_command("-t mangle -A " CHAIN_TRUSTED " -s %s -j MARK --set-mark %d", client->ip, FW_MARK_KNOWN);
	}
}

void release_client_del(struct release_client *client)
{
	debug(LOG_INFO, "RELEASE CLIENT DEL");
	iptables_do_command("-t mangle -D " CHAIN_TRUSTED " -s %s -j MARK --set-mark %d", client->ip, FW_MARK_KNOWN);
	HASH_DEL(head, client);
	free(client->ip);
	free(client->mac);
	free(client->token);
	free(client);
}

static void client_allow(struct release_client *client)
{
	t_client *tmp;

	LOCK_CLIENT_LIST();
	if ((tmp = client_list_find(client->ip, client->mac)) == NULL) {
		debug(LOG_DEBUG, "New client for %s", client->ip);
		client_list_add(client->ip, client->mac, client->token);
	}
	
	tmp = client_list_find(client->ip, client->mac);
	fw_allow(tmp, FW_MARK_KNOWN);

	UNLOCK_CLIENT_LIST();
}

void release_client_timeout(void)
{
	struct release_client *cur;
	struct release_client *tmp;
	t_authresponse authresponse;

	HASH_ITER(hh, head, cur, tmp) {
		debug(LOG_INFO, "FOUND");
		if (time(NULL) - cur->time < cur->timeout)
			return;
		/* send login request with a determined token */
		auth_server_request(&authresponse, REQUEST_TYPE_LOGIN, cur->ip, cur->mac, cur->token, 0, 0, 0, 0);
		if (authresponse.authcode == AUTH_ALLOWED)
			client_allow(cur);
		release_client_del(cur);
	}
}
