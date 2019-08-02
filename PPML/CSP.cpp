//
// Created by root on 30/06/19.
//

#include "CSP.h"

CSP::CSP(void) {
    // Creating socket file descriptor
    int valread;
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
    {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

// Forcefully attaching socket to the port 8080
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR,
                   &opt, sizeof(opt)))
    {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(port);

// Forcefully attaching socket to the port 8080
    if (::bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("bind error");
    }

    if (listen(server_fd, 3) < 0)
    {
        perror("listen");
        exit(EXIT_FAILURE);
    }
    if ((new_socket = accept(server_fd, (struct sockaddr *)&address,
                             (socklen_t*)&addrlen))<0)
    {
        perror("accept");
        exit(EXIT_FAILURE);
    }
}




bool send_data(int sock, void *buf, int buflen)
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


bool send_long(int sock, long value)
{
    value = htonl(value);
    return send_data(sock, &value, sizeof(value));
}



bool CSP::send_file(int sock, char* path)
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



bool read_data(int sock, void *buf, int buflen)
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

bool read_long(int sock, long *value)
{
    if (!read_data(sock, value, sizeof(value)))
        return false;
    *value = ntohl(*value);
    return true;
}


bool CSP::read_file(int sock, char* path)
{
    FILE *f = fopen(path, "wb");
    long filesize;
    if (!read_long(sock, &filesize)) {
        cout << "lol" << endl;
        return false;
    }
    if (filesize > 0)
    {
        cout << "Getting a file of size : " << filesize << endl;
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

void CSP::test_read_file() {
    read_file(new_socket, "test_write_cipher.txt");
}

void CSP::test_key_exchange() {

    // Key generation part

    long logp = 30; ///< Scaling Factor (larger logp will give you more accurate value)
    long logn = 10; ///< number of slot is 1024 (this value should be < logN in "src/Params.h")
    long logq = 300; ///< Ciphertext modulus (this value should be <= logQ in "scr/Params.h")
    long n = 1 << logn;
    long numThread = 2;

    srand(42);
    SetNumThreads(numThread);
    Ring ring;
    SecretKey secretKey(ring);
    Scheme scheme(secretKey, ring);
    scheme.addEncKey(secretKey);
    scheme.addMultKey(secretKey);

    scheme.addLeftRotKeys(secretKey); ///< When you need left rotation for the vectorized message
    scheme.addRightRotKeys(secretKey); ///< When you need right rotation for the vectorized message

    /*scheme.read_encryption_key();
    scheme.read_multiplication_key();
    scheme.read_left_rotation_key();
    scheme.read_right_rotation_key();*/
    long val;

    if (read_long(new_socket, &val)) {
        cout << "Got the number : " << val << endl;
    }

    // Send the cryptographic keys
    if (send_file(new_socket, "serkey/ENCRYPTION_KEY.txt")) {
        cout << "Okay, now send the keys" << endl;

        // Now, get the test cipher
        read_file(new_socket, "cipher_test.txt");

        cout << "Already read the test cipher" << endl;

        Ciphertext* test_cipher = SerializationUtils::readCiphertext("cipher_test.txt");
        complex<double> *decrypted_cipher1 = scheme.decrypt(secretKey, *test_cipher);

        cout << "Decryption test cipher:" << endl;
        for (int i = 0; i < n; ++i) {
            cout << decrypted_cipher1[i] << ' ';
        }
        cout << " " << endl;

        // Send the multiplication key

        if (send_file(new_socket, "serkey/MULTIPLICATION_KEY.txt")) {
            // Now, we test the multiplication

            read_file(new_socket, "cipher_mult.txt");
            Ciphertext* mult_cipher = SerializationUtils::readCiphertext("cipher_mult.txt");
            complex<double> *decrypted_mult_cipher = scheme.decrypt(secretKey, *mult_cipher);

            cout << "Decryption mult cipher:" << endl;
            for (int i = 0; i < n; ++i) {
                cout << decrypted_mult_cipher[i] << ' ';
            }
            cout << " " << endl;
        }


        // Send the left rotation keys
        for (long i = 0; i < logN - 1; ++i) {
            cout << "Rot " << i << endl;
            long idx = 1 << i;
            char* path = (char*) scheme.serKeyMap.at(idx).c_str();
            if(send_file(new_socket, path)) {

            } else {
                cout << "ERROR : COULD NOT SEND THE ROTATION KEY" << endl;
            }
        }

        read_file(new_socket, "cipher_rot.txt");
        Ciphertext* left_rot_cipher = SerializationUtils::readCiphertext("cipher_rot.txt");
        complex<double> *decrypted_left_rot_cipher = scheme.decrypt(secretKey, *left_rot_cipher);

        cout << "Decryption rot cipher:" << endl;
        for (int i = 0; i < n; ++i) {
            cout << decrypted_left_rot_cipher[i] << ' ';
        }
        cout << " " << endl;

        // Send right rotation keys
        for (long i = 0; i < logN - 1; ++i) {
            cout << "Rot " << i << endl;
            long idx = Nh - (1 << i);
            char* path = (char*) scheme.serKeyMap.at(idx).c_str();
            if(send_file(new_socket, path)) {

            } else {
                cout << "ERROR : COULD NOT SEND THE ROTATION KEY" << endl;
            }
        }
        read_file(new_socket, "cipher_right_rot.txt");

        Ciphertext* right_rot_cipher = SerializationUtils::readCiphertext("cipher_right_rot.txt");
        complex<double> *decrypted_right_rot_cipher = scheme.decrypt(secretKey, *right_rot_cipher);

        cout << "Decryption right rot cipher:" << endl;
        for (int i = 0; i < n; ++i) {
            cout << decrypted_right_rot_cipher[i] << ' ';
        }
        cout << " " << endl;


        } else {
        cout << "ERROR : COULD NOT SEND THE ENCRYPTION KEY" << endl;
    }


    // Send the keys

    // Encryption key

}