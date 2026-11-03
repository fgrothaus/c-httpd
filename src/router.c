#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../include/router.h"

const char* get_mime_type(const char *path) {
    const char *dot = strrchr(path, '.');
    if (!dot) {
        return "application/octet-stream";
    }

    if (strcmp(dot, ".html") == 0 || strcmp(dot, ".htm") == 0) return "text/html; charset=utf-8";
    if (strcmp(dot, ".css")  == 0) return "text/css";
    if (strcmp(dot, ".js")   == 0) return "application/javascript";
    if (strcmp(dot, ".png")  == 0) return "image/png";
    if (strcmp(dot, ".jpg")  == 0 || strcmp(dot, ".jpeg") == 0) return "image/jpeg";
    if (strcmp(dot, ".gif")  == 0) return "image/gif";
    if (strcmp(dot, ".txt")  == 0) return "text/plain; charset=utf-8";

    return "application/octet-stream"; // Standard Binärdaten
}


void add_response_header(HttpResponse *response, const char* key, const char* value) {
    if (response->header_counter < MAX_HEADERS) {
        response->headers[response->header_counter].key = (char*)key;
        response->headers[response->header_counter].value = (char*)value;
        response->header_counter++;
    }
}


int load_file_content(const char* request_path, char** out_body, size_t* out_size) {

    if (request_path == NULL || out_body == NULL || out_size == NULL) return -1;

    char full_path[512]; // Der request_path enthält sowas wie / oder /style.css und wird dem Ordner, in dem die vom Webserver bereitgestellten Dateien liegen zugeordnet

    if (strcmp(request_path, "/") == 0) {
        snprintf(full_path, sizeof(full_path), "public/index.html");
    } else {
        snprintf(full_path, sizeof(full_path), "public%s", request_path);
    }

    // ******************** Datei lesen und Größe ermitteln ********************
    FILE *file = fopen(full_path, "rb");
    if (file == NULL) {
        return -1;
    }

    // Dateigröße ermitteln:
    // Ans Ende der Datei springen
    if (fseek(file, 0, SEEK_END) != 0) {
        fclose(file);
        return -1;
    }
    
    long file_size = ftell(file); // Gibt long zurück, damit -1 Fehler zeigen kann. Direkt als size_t geht nicht, wegen unsigned
    if (file_size < 0) {
        fclose(file);
        return -1;
    }

    rewind(file); // An den Dateianfang springen

    // ******************** Inhalt der Datei  ********************
    char* buffer = malloc((size_t)file_size + 1); // +1 für String-Ende \0
    if (buffer == NULL) {
        fclose(file);
        return -1; // Out of Memory
    }

    // Gibt die Anzahl an Bytes zurück, die gelesen wurden und kopiert sie in buffer.
    size_t bytes_read = fread(buffer, 1, (size_t)file_size, file); // 1 = Größe eines einzelnen Elements in Bytes (also Char). (size_t)file_size gibt an, wie viele Elemente gelesen werden sollen
    fclose(file);

    if (bytes_read != (size_t)file_size) {
        free(buffer);
        return -1; // Lesefehler
    }

    buffer[file_size] = '\0'; // file_size ist der +1te Wert, wegen Index

    *out_body = buffer; // *out_body ist der dereferenzierte Wert des **, also der Pointer der in route_request auf NULL zeigt
    *out_size = (size_t)file_size;

    return 0;
}



void route_request(const HttpRequest *request, HttpResponse *response) {

    if (request == NULL || response == NULL) return;
    memset(response, 0, sizeof(HttpResponse));


    if (strstr(request->path, "..")) {
            response->status_code = 403;
            response->status_msg = "Forbidden";
            response->body = NULL;
            response->body_length = 0;
            add_response_header(response, "Content-Type", "text/html; charset=utf-8");
            return;
        }

    if (strcmp(request->method, "GET") == 0) {

        size_t file_size = 0;
        char* file_data = NULL;
        int status = load_file_content(request->path, &file_data, &file_size);

        if (status == 0) {
            response->status_code = 200;
            response->status_msg = "OK";
            response->body = file_data;
            response->body_length = file_size;
            
            const char* mime = (strcmp(request->path, "/") == 0) ? "text/html; charset=utf-8" : get_mime_type(request->path);

            add_response_header(response, "Content-Type", mime);

        } else {
            response->status_code = 404;
            response->status_msg = "Not Found";
            response->body = "<h1>404 Not Found</h1>";
            response->body_length = strlen(response->body);

            add_response_header(response, "Content-Type", "text/html; charset=utf-8");
        }
    } else if (strcmp(request->method, "HEAD") == 0) {

        size_t file_size = 0;
        char* file_data = NULL;
        int status = load_file_content(request->path, &file_data, &file_size);

        if (status == 0) {
            response->status_code = 200;
            response->status_msg = "OK";
            response->body = NULL;
            response->body_length = file_size;
            free(file_data);
            
            const char* mime = (strcmp(request->path, "/") == 0) ? "text/html; charset=utf-8" : get_mime_type(request->path);

            add_response_header(response, "Content-Type", mime);

        } else {
            response->status_code = 404;
            response->status_msg = "Not Found";
            response->body = NULL;
            response->body_length = 0;

            add_response_header(response, "Content-Type", "text/html; charset=utf-8");
        }

    } else {
        response->status_code = 405;
        response->status_msg = "Method not allowed";
        response->body = NULL;
        response->body_length = 0;

        add_response_header(response, "Allow", "GET, HEAD");
    }
}
