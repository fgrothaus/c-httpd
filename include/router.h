#ifndef ROUTER_H
#define ROUTER_H

#include "http.h"

void route_request(const HttpRequest *req, HttpResponse *res); 
void add_response_header(HttpResponse *response, const char* key, const char* value);
#endif