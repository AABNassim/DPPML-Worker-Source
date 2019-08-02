//
// Created by root on 30/06/19.
//

#include "Worker.h"

Worker::Worker() {

}

void Worker::connect_to_mlsp() {
    if ((mlsp_sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        printf("\n Socket creation error \n");
        return;
    }

    mlsp_addr.sin_family = AF_INET;
    mlsp_addr.sin_port = htons(mlsp_port);

    // Convert IPv4 and IPv6 addresses from text to binary form
    if(inet_pton(AF_INET, mlsp_ip, &mlsp_addr.sin_addr)<=0)
    {
        printf("\nInvalid address/ Address not supported \n");
        return;
    }

    if (connect(mlsp_sock, (struct sockaddr *)&mlsp_addr, sizeof(mlsp_addr)) < 0)
    {
        printf("\nConnection Failed \n");
        return;
    }
    send_long(mlsp_sock, 22);
}

void Worker::send_message() {
    send(mlsp_sock , hello, strlen(hello) , 0 );

    printf("Hello message sent\n");
    int valread = read(mlsp_sock , buffer, 1024);
    printf("%s\n", buffer);
}

bool Worker::send_data(int sock, void *buf, int buflen)
{
    unsigned char *pbuf = (unsigned char *) buf;

    while (buflen > 0)
    {
        int num = send(sock, pbuf, buflen, 0);
        pbuf += num;
        buflen -= num;
    }

    return true;
}


bool Worker::send_long(int sock, long value)
{
    value = htonl(value);
    return send_data(sock, &value, sizeof(value));
}



bool Worker::send_file(int sock, char* path)
{
    FILE *f = fopen(path, "rb");
    fseek(f, 0, SEEK_END);
    long filesize = ftell(f);
    rewind(f);
    if (filesize == EOF)
        return false;
    if (!send_long(sock, filesize))
        return false;
    if (filesize > 0)
    {
        char buffer[1024];
        do
        {
            size_t num = (filesize < sizeof(buffer)) ?  filesize : sizeof(buffer);
            num = fread(buffer, 1, num, f);
            if (num < 1)
                return false;
            if (!send_data(sock, buffer, num))
                return false;
            filesize -= num;
        }
        while (filesize > 0);
    }
    return true;
}


bool Worker::read_data(int sock, void *buf, int buflen)
{
    unsigned char *pbuf = (unsigned char *) buf;

    while (buflen > 0)
    {
        int num = recv(sock, pbuf, buflen, 0);
        if (num == 0)
            return false;

        pbuf += num;
        buflen -= num;
    }

    return true;
}

bool Worker::read_long(int sock, long *value)
{
    if (!read_data(sock, value, sizeof(value)))
        return false;
    *value = ntohl(*value);
    return true;
}


bool Worker::read_file(int sock, char* path)
{
    FILE *f = fopen(path, "wb");
    long filesize;
    if (!read_long(sock, &filesize))
        return false;
    if (filesize > 0)
    {
        char buffer[1024];
        do
        {
            int num = (filesize < sizeof(buffer)) ?  filesize : sizeof(buffer);
            if (!read_data(sock, buffer, num))
                return false;
            int offset = 0;
            do
            {
                size_t written = fwrite(&buffer[offset], 1, num-offset, f);
                if (written < 1)
                    return false;
                offset += written;
            }
            while (offset < num);
            filesize -= num;
        }
        while (filesize > 0);
    }
    return true;
}


