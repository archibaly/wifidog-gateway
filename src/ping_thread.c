/* vim: set sw=4 ts=4 sts=4 et : */
/********************************************************************\
 * This program is free software; you can redistribute it and/or    *
 * modify it under the terms of the GNU General Public License as   *
 * published by the Free Software Foundation; either version 2 of   *
 * the License, or (at your option) any later version.              *
 *                                                                  *
 * This program is distributed in the hope that it will be useful,  *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of   *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the    *
 * GNU General Public License for more details.                     *
 *                                                                  *
 * You should have received a copy of the GNU General Public License*
 * along with this program; if not, contact:                        *
 *                                                                  *
 * Free Software Foundation           Voice:  +1-617-542-5942       *
 * 59 Temple Place - Suite 330        Fax:    +1-617-542-2652       *
 * Boston, MA  02111-1307,  USA       gnu@gnu.org                   *
 *                                                                  *
\********************************************************************/

/* $Id$ */
/** @file ping_thread.c
    @brief Periodically checks in with the central auth server so the auth
    server knows the gateway is still up.  Note that this is NOT how the gateway
    detects that the central server is still up.
    @author Copyright (C) 2004 Alexandre Carmel-Veilleux <acv@miniguru.ca>
*/

#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <stdarg.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include <syslog.h>
#include <signal.h>
#include <errno.h>

#include "../config.h"
#include "safe.h"
#include "common.h"
#include "conf.h"
#include "debug.h"
#include "ping_thread.h"
#include "util.h"
#include "centralserver.h"
#include "firewall.h"
#include "gateway.h"
#include "simple_http.h"

static void ping(void);

/** Launches a thread that periodically checks in with the wifidog auth server to perform heartbeat function.
@param arg NULL
@todo This thread loops infinitely, need a watchdog to verify that it is still running?
*/
void
thread_ping(void *arg)
{
    pthread_cond_t cond = PTHREAD_COND_INITIALIZER;
    pthread_mutex_t cond_mutex = PTHREAD_MUTEX_INITIALIZER;
    struct timespec timeout;

    while (1) {
        /* Make sure we check the servers at the very begining */
        debug(LOG_DEBUG, "Running ping()");
        ping();

        /* Sleep for config.checkinterval seconds... */
        timeout.tv_sec = time(NULL) + config_get_config()->checkinterval;
        timeout.tv_nsec = 0;

        /* Mutex must be locked for pthread_cond_timedwait... */
        pthread_mutex_lock(&cond_mutex);

        /* Thread safe "sleep" */
        pthread_cond_timedwait(&cond, &cond_mutex, &timeout);

        /* No longer needs to be locked */
        pthread_mutex_unlock(&cond_mutex);
    }
}

int
got_pong_value(const char *str, const char *key, char *value, int size)
{
    int i;
    int offset = 0;
    char tmp[512];
    char key_tmp[64];
    snprintf(key_tmp, sizeof(key_tmp) - 1, "%s=", key);

    while (str[offset] != '\0') {
        for (i = 0; str[i + offset] != '\n' && str[i + offset] != '\0'; i++)
            tmp[i] = str[i + offset];
        tmp[i] = '\0';
        offset = offset + i + 1;
        if (strstr(tmp, key_tmp)) {
            strncpy(value, tmp + strlen(key_tmp), size - 1);
            return 1;
        }
    }
    return 0;
}

void
check_config_version(const char *res, const t_auth_serv *auth_server)
{
    char conf_ver[16];
    char request[MAX_BUF];

    if (got_pong_value(res, "conf_ver", conf_ver, sizeof(conf_ver))) {
        debug(LOG_INFO, "conf_ver=%s", conf_ver);
        if (atoi(conf_ver) > wifidog_cfg_version) { /* newer than me */
            int sockfd = connect_auth_server();
            if (sockfd == -1) {
                debug(LOG_ERR, "connect auth server error!");
                return;
            }
            /* send conf request */
            snprintf(request, sizeof(request) - 1,
                     "GET %sconf/?gw_id=%s&version=%d HTTP/1.0\r\n"
                     "User-Agent: WiFiDog %s\r\n"
                     "Host: %s\r\n"
                     "\r\n",
                     auth_server->authserv_path,
                     config_get_config()->gw_id,
                     wifidog_cfg_version,
                     VERSION,
                     auth_server->authserv_hostname);
            debug(LOG_INFO, "conf request = %s", request);
            char *res;
        #ifdef USE_CYASSL
            if (auth_server->authserv_use_ssl) {
                res = https_get(sockfd, request, auth_server->authserv_hostname);
            } else {
                res = http_get(sockfd, request);
            }
        #endif
        #ifndef USE_CYASSL
            res = http_get(sockfd, request);
        #endif
            debug(LOG_INFO, "conf response = %s", res);
            // wifidog_cfg_version = atoi(conf_ver);
        }
    }
}

static inline int
find_char(const char *str, char ch)
{
    int i;
    for (i = 0; str[i] != '\0'; i++) {
        if (str[i] == ch)
            return i;
    }
    return -1;
}

/*
 * example:
 *   res = "192.168.1.3|00:0c:eb:10:87:21;192.168.1.5|ea:21:09:65:20:81";
 */
int
pong_got_ip_mac(const char *res, int num, struct client *client)
{
    int i;
    int pos = 0;
    int tmp = 0;

    for (i = 0; i < num; i++) {
        /* get ip */
        tmp = pos;
        pos += find_char(res + pos, '|');
        strncpy(client[i].ip, res + tmp, pos - tmp);
        pos++;

        /* get mac */
        tmp = pos;
        if (i == num - 1) { /* no ';' for the last one */
            strncpy(client[i].mac, res + tmp, strlen(res) - tmp);
            return 1;
        } else {
            pos += find_char(res + pos, ';');
            strncpy(client[i].mac, res + tmp, pos - tmp);
        }
        pos++;
    }
    return 0;
}

void
check_logout(const char *res)
{
    char client_num[16];
    char client_list[512];

    if (got_pong_value(res, "client_num", client_num, sizeof(client_num))) {
        got_pong_value(res, "client_list", client_list, sizeof(client_list));
        int client_num_i = atoi(client_num);
        struct client *pong_client = safe_malloc(client_num_i * sizeof(struct client));
        pong_got_ip_mac(client_list, client_num_i, pong_client);
        int i;
        for (i = 0; i < client_num_i; i++) {
            LOCK_CLIENT_LIST();
            t_client *client = client_list_find_by_ip(pong_client[i].ip);
            if (client) {
                fw_deny(client);
                client_list_delete(client);
            }
            UNLOCK_CLIENT_LIST();
        }
        free(pong_client);
    }
}

/** @internal
 * This function does the actual request.
 */
static void
ping(void)
{
    char request[MAX_BUF];
    FILE *fh;
    int sockfd;
    unsigned long int sys_uptime = 0;
    unsigned int sys_memfree = 0;
    float sys_load = 0;
    t_auth_serv *auth_server = NULL;
    auth_server = get_auth_server();
    static int authdown = 0;

    debug(LOG_DEBUG, "Entering ping()");
    memset(request, 0, sizeof(request));

    /*
     * The ping thread does not really try to see if the auth server is actually
     * working. Merely that there is a web server listening at the port. And that
     * is done by connect_auth_server() internally.
     */
    sockfd = connect_auth_server();
    if (sockfd == -1) {
        /*
         * No auth servers for me to talk to
         */
        if (!authdown) {
            fw_set_authdown();
            authdown = 1;
        }
        return;
    }

    /*
     * Populate uptime, memfree and load
     */
    if ((fh = fopen("/proc/uptime", "r"))) {
        if (fscanf(fh, "%lu", &sys_uptime) != 1)
            debug(LOG_CRIT, "Failed to read uptime");

        fclose(fh);
    }
    if ((fh = fopen("/proc/meminfo", "r"))) {
        while (!feof(fh)) {
            if (fscanf(fh, "MemFree: %u", &sys_memfree) == 0) {
                /* Not on this line */
                while (!feof(fh) && fgetc(fh) != '\n') ;
            } else {
                /* Found it */
                break;
            }
        }
        fclose(fh);
    }
    if ((fh = fopen("/proc/loadavg", "r"))) {
        if (fscanf(fh, "%f", &sys_load) != 1)
            debug(LOG_CRIT, "Failed to read loadavg");

        fclose(fh);
    }

    /*
     * Prep & send request
     */
    snprintf(request, sizeof(request) - 1,
             "GET %s%sgw_id=%s&sys_uptime=%lu&sys_memfree=%u&sys_load=%.2f&wifidog_uptime=%lu&version=%d HTTP/1.0\r\n"
             "User-Agent: WiFiDog %s\r\n"
             "Host: %s\r\n"
             "\r\n",
             auth_server->authserv_path,
             auth_server->authserv_ping_script_path_fragment,
             config_get_config()->gw_id,
             sys_uptime,
             sys_memfree,
             sys_load,
             (long unsigned int)((long unsigned int)time(NULL) - (long unsigned int)started_time),
             wifidog_cfg_version,
             VERSION, auth_server->authserv_hostname);
    debug(LOG_INFO, "ping request = %s", request);
    char *res;
#ifdef USE_CYASSL
    if (auth_server->authserv_use_ssl) {
        res = https_get(sockfd, request, auth_server->authserv_hostname);
    } else {
        res = http_get(sockfd, request);
    }
#endif
#ifndef USE_CYASSL
    res = http_get(sockfd, request);
#endif
    debug(LOG_INFO, "ping response = %s", res);
    if (NULL == res) {
        debug(LOG_ERR, "There was a problem pinging the auth server!");
        if (!authdown) {
            fw_set_authdown();
            authdown = 1;
        }
    } else if (!strstr(res, "Pong")) {
        debug(LOG_WARNING, "Auth server did NOT say Pong!");
        if (!authdown) {
            fw_set_authdown();
            authdown = 1;
        }
        free(res);
    } else {
        debug(LOG_INFO, "Auth Server Says: Pong");
        if (authdown) {
            fw_set_authup();
            authdown = 0;
        }
        check_config_version(res, auth_server);
        check_logout(res);
        free(res);
    }
    return;
}
