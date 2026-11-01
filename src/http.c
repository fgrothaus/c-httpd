#include <stdio.h>
#include <string.h>
#include <strings.h>

#include "../include/http.h"

// G E T _ / i n d e x . h t m l _ H T T P / 1 . 1 \r \n H o s t : _ l o c a l h o s t \r \n \r \n [BODY]
// strstr() - Sucht nach Wörtern in einem String
// strchr() - Sucht nach einzelnen Zeichen in einem String
void print_http_request(const HttpRequest *req) {
    if (req == NULL) {
        printf("[HttpRequest is NULL]\n");
        return;
    }

    printf("========== HTTP REQUEST ==========\n");
    printf("Method:  %s\n", req->method ? req->method : "(null)");
    printf("Path:    %s\n", req->path ? req->path : "(null)");
    printf("Version: %s\n", req->version ? req->version : "(null)");
    printf("----------------------------------\n");

    printf("Headers (%zu):\n", req->header_counter);
    for (size_t i = 0; i < req->header_counter; i++) {
        printf("  [%zu] %s: %s\n", i, req->headers[i].key, req->headers[i].value);
    }
    printf("----------------------------------\n");

    printf("Body Length: %zu Bytes\n", req->body_length);
    if (req->body != NULL && req->body_length > 0) {
        // %.*s druckt exakt req->body_length Zeichen (auch wenn kein \0 am Ende des Bodys liegt)
        printf("Body:\n%.*s\n", (int)req->body_length, req->body);
    } else {
        printf("Body: (empty)\n");
    }
    printf("==================================\n\n");
    fflush(stdout);
}

int parse_http_request(char* buffer, size_t len, HttpRequest* req) {

    //********************************************* Body abtrennen *********************************************
    char* body_sep = strstr(buffer, "\r\n\r\n");
    if (body_sep == NULL) {
        return -1; // NULL, wenn \r\n\r\n nicht gefunden wurde => Kein valider HTTP-Request
    }

    char* body_start = body_sep + 4; // Die 4 bezieht sich für den Compiler auf die Größe des Datentyps, auf den der Pointer zeigt.
    req->body = body_start;

    *body_sep = '\0'; // Erste Adresse (\r) überschreiben mit String-Ende, damit andere String-Funktionen den Body ignorieren.
    
    size_t metadata_len = (size_t)(body_start - buffer); // Adressen verrechnen. Buffer=Startadresse z.B. 1000, body_start 1300 => 1300-1000=300 Bytes Abstand
    req->body_length = len - metadata_len; // Gesamtanzahl Bytes z.B. 500 - 300 = 200 Bytes

    //printf("%s\n", req->body);


    //********************************************* Request-Line extrahieren *********************************************
    char* req_line_end = strstr(buffer, "\r\n");
    if (req_line_end == NULL) {
        return -1;
    }
    

    req->method = buffer; // Erstmal die Method auf buffer-Adresse senden und danach abknipsen durch \0

    char* space_after_method = strchr(buffer, ' ');
    if (space_after_method == NULL || space_after_method > req_line_end) {
        return -1; // Format kaputt, keine Leerzeichen in der Request-Line
    }
    *space_after_method = '\0';
    // printf("%s\n", req->method);


    req->path = space_after_method + 1;

    char* space_after_path = strchr((space_after_method + 1), ' ');
    if (space_after_path == NULL || space_after_path > req_line_end) {
        return -1; // Format kaputt, keine Leerzeichen in der Request-Line
    }
    *space_after_path = '\0';
    // printf("%s\n", req->path);

    
    req->version = space_after_path + 1;

    char* new_line_after_version = strchr((space_after_path + 1), '\r');
    if (new_line_after_version == NULL || new_line_after_version > req_line_end) {
        return -1; // Format kaputt, keine Leerzeichen in der Request-Line
    }
    *new_line_after_version = '\0';
    // printf("%s\n", req->version);


    char* header_start = req_line_end + 2;
    //printf("%s\n", header_start);



    //********************************************* Http-Header extrahieren *********************************************
    char* curr = header_start;
    req->header_counter = 0;

    while (curr < body_sep && *curr != '\0' && req->header_counter < MAX_HEADERS) {

        char* header_eol = strstr(curr, "\r\n");
        if (header_eol == NULL) {
            break;
        }

        //header_eol* = '\0';
        
        char* colon = strchr(curr, ':');
        if (colon == NULL || colon > header_eol) { // colon > header_eol z.B. 0x200 > 0x210, also Header Ende muss immer nach colon sein, sonst wird über den Buffer hinaus geprüft
            return -1; // Ungültiger Header
        }
        
        *colon = '\0';
        req->headers[req->header_counter].key = curr;

        char* value_start = colon + 1;
        if (*value_start == ' ') {
            value_start++; // Führendes Leerzeichen nach Doppelpunkt überspringen
        }

        //char* test = strchr(value_start, header_eol);
        *header_eol = '\0';
        req->headers[req->header_counter].value = value_start;


        curr = header_eol + 2;
        req->header_counter++;
    }

    // Für HTTP/1.1 ist Host ein Pflicht-Header
    if (strcmp(req->version, "HTTP/1.1") == 0) {
        int host_found = 0;
        for (size_t i = 0; i < req->header_counter; i++) {
            // Case-insensitive Vergleich, da Header-Namen im Standard case-insensitiv sind
            if (strcasecmp(req->headers[i].key, "Host") == 0) {
                host_found = 1;
                break;
            }
        }
        if (!host_found) {
            return -1;
        }
    }

    if (req->method[0] == '\0' || req->path[0] == '\0' || req->version[0] == '\0') {
        return -1;
    }

    print_http_request(req);

    return 0;
}


int build_http_response(const HttpResponse* res, char* out_buffer, size_t out_capacity) {
    
    if (res == NULL || out_buffer == NULL || out_capacity == 0) {
        return -1;
    }

    return 0;
}


// #include <stdio.h>
// #include <string.h>
// #include "../include/http.h"

// int build_http_response(const HttpResponse* res, char* out_buffer, size_t out_capacity) {
//     if (res == NULL || out_buffer == NULL || out_capacity == 0) {
//         return -1;
//     }

//     size_t offset = 0; // Merkt sich, an welcher Byte-Position wir gerade schreiben

//     // 1. Status-Line schreiben (z. B. "HTTP/1.1 200 OK\r\n")
//     int written = snprintf(out_buffer + offset, out_capacity - offset,
//                            "HTTP/1.1 %d %s\r\n",
//                            res->status_code,
//                            res->status_msg);

//     if (written < 0 || (size_t)written >= (out_capacity - offset)) {
//         return -1; // Puffer reicht nicht
//     }
//     offset += (size_t)written;

//     // 2. Alle Header aus dem Struct iterieren und anhängen
//     for (size_t i = 0; i < res->header_counter; i++) {
//         written = snprintf(out_buffer + offset, out_capacity - offset,
//                            "%s: %s\r\n",
//                            res->headers[i].key,
//                            res->headers[i].value);

//         if (written < 0 || (size_t)written >= (out_capacity - offset)) {
//             return -1; // Puffer reicht nicht
//         }
//         offset += (size_t)written;
//     }

//     // 3. Content-Length & Leerzeile setzen (schließt Header-Block ab)
//     written = snprintf(out_buffer + offset, out_capacity - offset,
//                        "Content-Length: %zu\r\nConnection: close\r\n\r\n",
//                        res->body_length);

//     if (written < 0 || (size_t)written >= (out_capacity - offset)) {
//         return -1; // Puffer reicht nicht
//     }
//     offset += (size_t)written;

//     // 4. Body anhängen
//     if (res->body != NULL && res->body_length > 0) {
//         if (offset + res->body_length > out_capacity) {
//             return -1; // Body passt nicht mehr in den Puffer
//         }
//         memcpy(out_buffer + offset, res->body, res->body_length);
//         offset += res->body_length;
//     }

//     // Gibt die Gesamtanzahl an Bytes zurück, die nun via send() gesendet werden müssen
//     return (int)offset;
// }