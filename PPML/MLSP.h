//
// Created by root on 30/06/19.
//

#ifndef DPPML_MLSP_H
#define DPPML_MLSP_H

#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>


class MLSP {
public:
    int sock = 0, valread;
    struct sockaddr_in serv_addr;
    char *hello = "Hello from client";
    char buffer[1024] = {0};
    int port = 8080;
    char *csp_ip = "127.0.0.1";
    MLSP();
};


#endif //DPPML_MLSP_H
