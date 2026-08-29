#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h> // AF_INET - Address Family: Internet; SOCK_STREAM
#include <netinet/in.h> // POSIX-Socket-API

#include "../include/server.h"

static const char *HTTP_RESPONSE = 
    "HTTP/1.1 200 OK\r\n"
    "Content-Type: text/plain\r\n"
    "Content-Length: 13\r\n"
    "Connection: close\r\n"
    "\r\n"
    "Hello, World!";


int server_start(int port) {
    
    int server_fd, client_fd;
    // sockaddr_in aus POSIX. Die structs halten Verbindungsdaten (IP-Adresse, Port, Protokollfamilie)
    struct sockaddr_in server_addr; // Auf welcher Schnittstelle (Netzwerkadapter) und welchem Port soll gelauscht werden?
    struct sockaddr_in client_addr; // Bleibt leer. Pointer darauf wird an accept() übergeben. Wenn Client sich verbindet, schreibt Linux-Kernel IP und Absender-Port in das Struct.
    socklen_t client_len = sizeof(client_addr); // Wird gemeinsam mit &client_addr über accept() an den Kernel übergeben, damit der Kernel weiß, wie groß maximal der Client-Wert IP und Port sein darf.
    char buffer[BUFFER_SIZE]; // Buffer für die eingehenden HTTP-Requests
    int opt = 1;

    //printf("%zu\n",sizeof(socklen_t)); // zu steht für size_t unsigned
    //printf("%d\n", client_len);

    server_fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP); // AF_INET Kernel soll IPv4 Protokoll nutzen. AF_INET6 für IPv6.; SOCK_STREAM für TCP/IP. UPD wäre SOCK_DGRAM
    server_fd = -1;
    if (server_fd < 0) {
        perror("ERROR: socket() fehlgeschlagen"); // perror gibt Fehlermeldung auf stderr, File Descriptor 2 aus. Bei Fehler z.B. ERROR: socket() fehlgeschlagen: Permission Denied
        return -1;
    }
    
    printf("%d\n", server_fd);

    return 99;
}