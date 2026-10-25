#include <stddef.h>

#ifndef HTTP_H
#define HTTP_H

#define MAX_HEADERS 32

typedef struct {
    char* key;
    char* value;
}HttpHeader;


typedef struct {
    char* method;
    char* path;
    char* version;
    HttpHeader headers[MAX_HEADERS];
    size_t header_counter;
    const char* body;
    size_t body_length;
} HttpRequest;



typedef struct {
    int status_code;
    const char* status_msg;
    HttpHeader headers[MAX_HEADERS];
    size_t header_counter;
    const char* body; // const heißt nur, dass Werte readonly sind
    size_t body_length;
}HttpResponse;


// size_t len ist die Anzahl an Bytes, die recv() in den Buffer geschrieben hat.
int parse_http_request(char* buffer, size_t len, HttpRequest* req); // C-Pattern ist, mit Pointern für veränderte Werte zu arbeiten, aber Statuswert (int) zu returnen
int build_http_response(const HttpResponse* res, char* out_buffer, size_t out_capacity);



#endif // HTTP_H