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

void handle_request(uint8_t *data, uint16_t length);
void fill_buffer(char *data);
void fill_buffer_n(char *data, uint16_t length);
void fill_buffer_p(const char *pdata);

void www_server_init(void) {
    // Register self to port
    tcp_port_register(EXT_WWW_SERVER_PORT, handle_request);
}

void www_server_register_path(uint8_t type, char *path, void (*callback)(uint8_t *data, uint16_t length)) {
  debug_string_p(PSTR("TODO"));
}

const char newline[]   PROGMEM = "\r\n";
const char not_found[] PROGMEM = "Not found";

uint8_t *rbuffer;
uint16_t rlength;

void handle_request(uint8_t *data, uint16_t length) {
    debug_string_p(PSTR("HTTP: "));

    // Prepare tcp reply
    rbuffer = tcp_prepare_reply();
    rlength = 0;
    // Increase ack with length
    add_value_to_buffer(length, &buffer_out[TCP_PTR_ACK_NR], 4);
    // Set ACK flag
    tcp_add_flags(TCP_FLAG_ACK | TCP_FLAG_PUSH | TCP_FLAG_FIN);

    // Check if we can handle the request
    if (0) {}
    else {
      // Return 404, not found
      //PSTR("HTTP/1.0 404 NOT FOUND\r\nContent-Type: text/html\r\nPragma: no-cache\r\n\r\n")
      www_server_reply(HTTP_STATUS_404, HTTP_CONTENT_TYPE_PLAIN, not_found);
    }

    debug_ok();
}

const char http_version[] PROGMEM = "HTTP/1.1 ";

const char http_status_404[] PROGMEM = "404 Not Found";

const char http_content_type_head[]  PROGMEM = "Content-Type: ";
const char http_content_type_plain[] PROGMEM = "text/plain";
const char http_content_type_html[]  PROGMEM = "text/html";
const char http_content_type_json[]  PROGMEM = "application/json";

void www_server_reply(uint8_t status, uint8_t content_type, const char *pdata) {
    // HTTP
    // --------------------------------------------------------------------

    // HTTP version
    fill_buffer_p(http_version);

    // Status
    if (0) {}
    else if (status == HTTP_STATUS_404) {
        fill_buffer_p(http_status_404);
    }

    // Newline
    fill_buffer_p(newline);

    // Headers
    // --------------------------------------------------------------------

    // Content type
    fill_buffer_p(http_content_type_head);
    if (0) {
    } else if (content_type == HTTP_CONTENT_TYPE_PLAIN) {
       fill_buffer_p(http_content_type_plain);
    } else if (content_type == HTTP_CONTENT_TYPE_HTML) {
        fill_buffer_p(http_content_type_html);
    } else if (content_type == HTTP_CONTENT_TYPE_JSON) {
        fill_buffer_p(http_content_type_json);
    }
    fill_buffer_p(newline);

    // End header with extra newline
    fill_buffer_p(newline);

    // Data
    // --------------------------------------------------------------------
    if (pdata) {
        fill_buffer_p(pdata);
        // Newlines to mark end of data
        fill_buffer_p(newline);
        fill_buffer_p(newline);
    }

    // Send packet
    tcp_send(rlength);
}

void fill_buffer(char *data) {
    fill_buffer_n(data, 0xFFFF);
}

void fill_buffer_n(char *data, uint16_t length) {
    while(*data && length--) {
        *rbuffer++ = *data++;
        rlength++;
    }
}

void fill_buffer_p(const char *pdata) {
    char c;
    while ((c = pgm_read_byte(pdata++))) {
      *rbuffer++ = c;
      rlength++;
    }
}

#endif // EXT_WWW_SERVER
