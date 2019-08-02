//
// Created by root on 30/06/19.
//

#include "MLSP.h"

MLSP::MLSP() {

    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        printf("\n Socket creation error \n");
        return;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(port);

    // Convert IPv4 and IPv6 addresses from text to binary form
    if(inet_pton(AF_INET, csp_ip, &serv_addr.sin_addr)<=0)
    {
        printf("\nInvalid address/ Address not supported \n");
        return;
    }

    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
        printf("\nConnection Failed \n");
        return;
    }
    /*send(sock , hello , strlen(hello) , 0 );
    printf("Hello message sent\n");
    valread = read(sock , buffer, 1024);
    printf("%s\n",buffer );*/
}



