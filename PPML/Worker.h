//
// Created by root on 30/06/19.
//

#ifndef DPPML_WORKER_H
#define DPPML_WORKER_H

#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string>
#include <string.h>
#include <stdlib.h>
#include <iostream>
#include "HEAAN.h"


class Worker {
public:
    int mlsp_sock = 0;
    struct sockaddr_in mlsp_addr;
    char *hello = "Hello from client";
    char buffer[1024] = {0};
    int mlsp_port = 8100;
    char *mlsp_ip = "127.0.0.1";

    Worker();
    void send_message();
    bool send_file(int sock, char* path);
    bool send_long(int sock, long value);
    bool send_data(int sock, void *buf, int buflen);


    bool read_file(int sock, char* path);
    bool read_long(int sock, long *value);
    bool read_data(int sock, void *buf, int buflen);

    void connect_to_mlsp();

};


#endif //DPPML_WORKER_H
