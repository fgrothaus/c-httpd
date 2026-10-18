#include <stdio.h>
#include <stdlib.h>
#include <string.h> // memset(), 
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

    // socket() gibt einen File-Descriptor zurück [0]->stdin [1]->stdout [2]->stderr [3]->socket. Ein File-Descriptor beschreibt eine Datenstruktur. Bei einem Socket, dass es z.B. TCP ist, IP und Ports, etc.
    // server_fd printet also eine 3, was immer der niedrigste freie Index des File-Descriptors ist.
    server_fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP); // AF_INET Kernel soll IPv4 Protokoll nutzen. AF_INET6 für IPv6.; SOCK_STREAM für TCP/IP. UPD wäre SOCK_DGRAM
    //server_fd = -1;
    if (server_fd < 0) {
        perror("ERROR: socket() fehlgeschlagen"); // perror gibt Fehlermeldung auf stderr, File Descriptor 2 aus. Bei Fehler z.B. ERROR: socket() fehlgeschlagen: Permission Denied
        return -1;
    }
    
    // Wenn ich den Webserver über Strg+C beende, würde das OS den Socket standardmäßig in den Zustand TIME_WAIT versetzen für 30-120 Sekunden.
    // Der Port, der an den Socket gebunden ist, steht also nicht mehr zur Verfügung für die Zeit.
    // setsockopt() ist das universälle Einstellungsmenü für einen Socket. Mit SO_REUSEADDR sage ich, dass der Socket freigegeben werden darf und mit close gebe ich tatsächlich frei.
    // Rückgabewert ist 0 (für Erfolg) oder -1 (bei Misserfolg) - POSIX-System-Call-Konvention sieht vor, dass POSIX-Syscalls, die keinen Wert zurückgeben müssen, immer entweder 0 oder -1 zurückgeben.
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) { // SOL_SOCKET=Flag für allgemeine Socket-Option, SO_REUSEADDR=Konkrete Option(Port wiederverwenden), &opt=true Flag, sizeof(opt)=Größe der von Ring 3 übergebenen Variable (I.d.R. anzugeben für den Kernel)
        perror("ERROR: setsockopt() fehlgeschlagen");
        close(server_fd); // Schließt sofort die Verbindung und gibt somit den Port wieder frei. Also setsockopt und close funktionieren gemeinsam.
        return -1;
    }
    
    memset(&server_addr, 0, sizeof(server_addr)); // Initialisieren des sockaddr_in Structs mit 0
    server_addr.sin_family = AF_INET; // Address Family: Internet, also IPv4
    server_addr.sin_addr.s_addr = INADDR_ANY; // Weist das Struct an, nicht nur an eine spezifische IP gebunden zu werden, sondern eingehende Pakete auf allen Netzwerk-Interfaces anzunehmen (Loopback, Ethernet, Wi-Fi)
    server_addr.sin_port = htons(port); // Kovertiert ggf. von Little- zu Bit-Endian, da x86-CPUs Daten per Little-Endian übertragen, aber TCP Big-Endian erwartet (sprich das größe Byte zuerst). Port 0-1023 sind previledged(well-known)-Ports => Admin PW eingeben

    // server_addr (Server Konfiguration) an Socket binden
    if (bind(server_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror("ERROR: bind() fehlgeschlagen");
        close(server_fd);
        return -1;
    }

    // Socket in den Listen-Modus versetzen
    // Der Parameter 10 gibt die maximale Anzahl an TCP-Connections an, die gepuffert werden, falls accept() noch nicht ausgeführt wurde.
    // Wenn das OS viel zutun hat, kann es sein, dass 10 Verbindungen aufgebaut wurden, aber noch nicht accept aufgerufen wurde, da die Anwendung durch den Scheduler noch nicht ausgeführt wurde.
    if (listen(server_fd, 10) < 0) {
        perror("ERROR: listen() fehlgeschlagen");
        close(server_fd);
        return -1;
    }

    printf("INFO: Server lauscht auf Port %d...\n", port);
    printf("INFO: Druecke Ctrl+C zum Beenden.\n\n");


    while(1) {
        
        client_fd = accept(server_fd, (struct sockaddr*)&client_addr, &client_len);

        if (client_fd < 0) {
            perror("WARNING: accept() fehlgeschlagen");
            continue;
        }

    }

    return 0;
}