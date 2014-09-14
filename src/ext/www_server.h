/**
 * @file www_server.h
 * @brief Small webserver
 *
 * Basic functions for a webserver.
 *
 * \copyright Copyright 2014 /Dev. All rights reserved.
 * \license This project is released under MIT license.
 *
 * @author Ferdi van der Werf <efcm@slashdev.nl>
 * @since 0.14.0
 */

#ifndef EXT_WWW_SERVER_H
#define EXT_WWW_SERVER_H

#include "../config.h"

// Do we want www_server?
#ifdef EXT_WWW_SERVER

#include <inttypes.h>
#include <avr/pgmspace.h>
#include "../net/tcp.h"

#define HTTP_METHOD_HEAD   0x01
#define HTTP_METHOD_GET    0x02
#define HTTP_METHOD_POST   0x03
#define HTTP_METHOD_PUT    0x04
#define HTTP_METHOD_DELETE 0x05

#define HTTP_STATUS_200 0x20
#define HTTP_STATUS_201 0x21
#define HTTP_STATUS_202 0x22
#define HTTP_STATUS_204 0x24
#define HTTP_STATUS_404 0x44

#define HTTP_CONTENT_TYPE_PLAIN 0x01
#define HTTP_CONTENT_TYPE_HTML  0x02
#define HTTP_CONTENT_TYPE_JSON  0x03

/**
 * @brief Initialize www server
 */
extern void www_server_init(void);

/**
 * @brief Register callback method on a path
 */
extern void www_server_register_path(const char *path, void (*callback)(uint8_t type, uint8_t *data));

/**
 * @brief Unregister callback method on a path
 */
extern void www_server_unregister_path(const char *path);

/**
 * @brief Reply to a http request
 */
extern void www_server_reply_header(uint8_t status, uint8_t content_type);

/**
 * @brief Send http request reply
 */
extern void www_server_reply_send();

extern void www_server_reply_add(char *data);
extern void www_server_reply_add_n(char *data, uint16_t length);
extern void www_server_reply_add_p(const char *pdata);

#endif // EXT_WWW_SERVER

#endif // EXT_WWW_SERVER_H
