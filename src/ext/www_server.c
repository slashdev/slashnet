/**
 * @file www_server.c
 *
 * \copyright Copyright 2014 /Dev. All rights reserved.
 * \license This project is released under MIT license.
 *
 * @author Ferdi van der Werf <efcm@slashdev.nl>
 * @since 0.14.0
 */

#include "www_server.h"

// Do we want www_server?
#ifdef EXT_WWW_SERVER

// Check if NET_TCP is enabled
#ifndef NET_TCP
#error EXT_WWW_SERVER cannot work without NET_TCP
#endif // NET_TCP

// Check if EXT_WWW_SERVER_PORT is set
#ifndef EXT_WWW_SERVER_PORT
#error EXT_WWW_SERVER requires EXT_WWW_SERVER_PORT to be set
#endif // EXT_WWW_SERVER_PORT

#ifndef EXT_WWW_SERVER_SERVICES_LIST_SIZE
#warning EXT_WWW_SERVER_SERVICES_LIST_SIZE not set, defaulting to 10
#define EXT_WWW_SERVER_SERVICES_LIST_SIZE 10
#endif

// Only build if requirements are met
#if defined(NET_TCP) && defined(EXT_WWW_SERVER_PORT)

void handle_request(uint8_t *data, uint16_t length);

// Path service
typedef struct {
  // Path to search for
  const char *path;
  // Callback function
  void (*callback)(uint8_t type, uint8_t *data);
} path_service_t;

path_service_t path_services[EXT_WWW_SERVER_SERVICES_LIST_SIZE];

uint8_t path_matches(uint8_t *str1, const char *str2) {
  char c;
  while (1) {
    // Get char from PROGMEM
    c = pgm_read_byte(str2);
    // Check for equality
    if (*str1 == 0 && c == 0) {
      // Both bytes are 0, thus equal
      return 1;
    } else if (*str1 != c) {
      // Bytes differ, thus unequal
      return 0;
    }
    // Move to next byte
    str1++;
    str2++;
  }
}

uint8_t path_matches_p(const char *str1, const char *str2) {
  char c, d;
  while (1) {
    // Get char from PROGMEM
    c = pgm_read_byte(str1++);
    d = pgm_read_byte(str2++);
    // Check for equality
    if (c == 0 && d == 0) {
      // Both bytes are 0, thus equal
      return 1;
    } else if (c != d) {
      // Bytes differ, thus unequal
      return 0;
    }
  }
}

void path_service_remove(const char *path);
void (*path_service_get(uint8_t *path, uint8_t length))(uint8_t type, uint8_t *data);
// Path service

void www_server_init(void) {
  // Register self to port
  tcp_port_register(EXT_WWW_SERVER_PORT, handle_request);

  // path_service_init();
  uint8_t i;
  for (i = 0; i < EXT_WWW_SERVER_SERVICES_LIST_SIZE; i++) {
    path_services[i].path = 0;
    path_services[i].callback = 0;
  }
}

void www_server_register_path(const char *path, void (*callback)(uint8_t type, uint8_t *data)) {
  // path_service_set(path, callback);
  uint8_t i;
  for (i = 0; i < EXT_WWW_SERVER_SERVICES_LIST_SIZE; i++) {
    if (path_services[i].path == 0) {
      path_services[i].path = path;
      path_services[i].callback = callback;
      return;
    }
  }
  debug_string_p(PSTR("Path service list full\r\n"));
}

void www_server_unregister_path(const char *path) {
  // path_service_remove(path);
  uint8_t i;
  for (i = 0; i < EXT_WWW_SERVER_SERVICES_LIST_SIZE; i++) {
    if (path_services[i].path && path_matches_p(path, path_services[i].path)) {
      path_services[i].path = 0;
      path_services[i].callback = 0;
      return;
    }
  }
  debug_string_p(PSTR("Path to remove not found\r\n"));
}

const char newline[]   PROGMEM = "\r\n";
const char not_found[] PROGMEM = "Not found";

uint8_t *rbuffer;
uint16_t rlength;

void handle_request(uint8_t *data, uint16_t length) {
  debug_string_p(PSTR("HTTP: "));

  uint8_t type = 0;
  uint8_t path_start = 0;
  switch(data[0]) {
    case 'H': type = HTTP_METHOD_HEAD;   path_start = 5; break;
    case 'G': type = HTTP_METHOD_GET;    path_start = 4; break;
    case 'D': type = HTTP_METHOD_DELETE; path_start = 7; break;
    case 'P':
      switch(data[1]) {
        case 'O': type = HTTP_METHOD_POST; path_start = 5; break;
        case 'U': type = HTTP_METHOD_PUT;  path_start = 4; break;
        default:  type = 0;
      }
      break;
  }

  // Fetch path only if we have a type
  if (type) {
    uint8_t path_length = 0;
    uint8_t *path = &data[path_start];
    while (*path++ > 0x20) {
      path_length++;
    }
    // Make space after path 0x00 (for path matching)
    data[path_start + path_length] = 0x00;
    // Debug path
    debug_string_n((char *)data, path_start + path_length);
  }

  // Get callback for type/path combination
  // path_services_get(&data[path_start]);
  uint8_t i;
  void (*callback)(uint8_t, uint8_t *);

  for (i = 0, callback = 0; i < EXT_WWW_SERVER_SERVICES_LIST_SIZE; i++) {
    if (path_services[i].path && path_matches(&data[path_start], path_services[i].path)) {
      callback = path_services[i].callback;
      break;
    }
  }

  // Prepare tcp reply
  rbuffer = tcp_prepare_reply();
  rlength = 0;
  // Increase ack with length
  add_value_to_buffer(length, &buffer_out[TCP_PTR_ACK_NR], 4);
  // Set ACK flag
  tcp_add_flags(TCP_FLAG_ACK | TCP_FLAG_PUSH | TCP_FLAG_FIN);

  // Check if we can handle the request
  if (callback) {
    callback(type, data);
  } else {
    // Return 404, not found
    www_server_reply_header(HTTP_STATUS_404, HTTP_CONTENT_TYPE_PLAIN);
    www_server_reply_add_p(not_found);
    www_server_reply_send();
  }

  debug_ok();
}

const char http_version[] PROGMEM = "HTTP/1.1 ";

const char http_status_200[] PROGMEM = "200 OK";
const char http_status_201[] PROGMEM = "201 Created";
const char http_status_202[] PROGMEM = "202 Accepted";
const char http_status_204[] PROGMEM = "204 No Content";
const char http_status_404[] PROGMEM = "404 Not Found";

const char http_content_type_head[]  PROGMEM = "Content-Type: ";
const char http_content_type_plain[] PROGMEM = "text/plain";
const char http_content_type_html[]  PROGMEM = "text/html";
const char http_content_type_json[]  PROGMEM = "application/json";

void www_server_reply_header(uint8_t status, uint8_t content_type) {
  // HTTP version
  www_server_reply_add_p(http_version);

  // Status
  if (0) {}
  else if (status == HTTP_STATUS_200) { www_server_reply_add_p(http_status_200); }
  else if (status == HTTP_STATUS_201) { www_server_reply_add_p(http_status_201); }
  else if (status == HTTP_STATUS_202) { www_server_reply_add_p(http_status_202); }
  else if (status == HTTP_STATUS_204) { www_server_reply_add_p(http_status_204); }
  else if (status == HTTP_STATUS_404) { www_server_reply_add_p(http_status_404); }

  // Newline
  www_server_reply_add_p(newline);

  // Headers
  // --------------------------------------------------------------------

  // Content type
  www_server_reply_add_p(http_content_type_head);
  if (0) { }
  else if (content_type == HTTP_CONTENT_TYPE_PLAIN) { www_server_reply_add_p(http_content_type_plain); }
  else if (content_type == HTTP_CONTENT_TYPE_HTML)  { www_server_reply_add_p(http_content_type_html);  }
  else if (content_type == HTTP_CONTENT_TYPE_JSON)  { www_server_reply_add_p(http_content_type_json);  }
  www_server_reply_add_p(newline);

  // End header with extra newline
  www_server_reply_add_p(newline);
}

void www_server_reply_send() {
  // Make sure a packet ends with two newlines to mark end of data
  www_server_reply_add_p(newline);
  www_server_reply_add_p(newline);
  // Send packet
  tcp_send(rlength);
}

void www_server_reply_add(char *data) {
  www_server_reply_add_n(data, 0xFFFF);
}

void www_server_reply_add_n(char *data, uint16_t length) {
  while(*data && length--) {
    *rbuffer++ = *data++;
    rlength++;
  }
}

void www_server_reply_add_p(const char *pdata) {
  char c;
  while ((c = pgm_read_byte(pdata++))) {
    *rbuffer++ = c;
    rlength++;
  }
}

#endif // NET_TCP && EXT_WWW_SERVER_PORT && EXT_WWW_SERVER_SERVICES_LIST_SIZE
#endif // EXT_WWW_SERVER
