#ifndef NETWORK_HPP
#define NETWORK_HPP

#include <vector>
#include <stdio.h>
#include <math.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>

#define PORT 5050

#include "util/info.hpp"

void send_msg(int sock, const char* msg) {
    send(sock, msg, strlen(msg), 0);
    char buffer[64];
    recv(sock, buffer, 64, 0);
}

void send_position(int sock, int id, float pos[3], float lookat[3]) {
    char buff[64];
    sprintf(buff, "+%d %f %f %f %f %f %f", id, pos[0], pos[1], pos[2],
                                        lookat[0], lookat[1],lookat[2]);
    send_msg(sock, buff);
}
void close_conn(int sock) {
    const char* msg = "-bye";
    send_msg(sock, msg);
    close(sock);
}
void* recieve_players_pos(void* data) {
    int (*arr)[2] = (int (*)[2]) data;
    int sock = arr[0][0];
    int tID = arr[0][1];
    std::cout << tID << "\n";
    char buffer[64]; 
    while(1) {
        recv(sock, buffer, 64, 0);
        std::cout << "-";
        if(buffer[0] != '\0') {
            std::cout << '\n';
            char* p;
            p = strtok(buffer," ");
            float info[6];
            int i = 0;
            int rID = atoi(p);
            if(rID == tID){ 
                std::cout << "works\n";
                continue;
            }
            std::cout << p << " ";
            p = strtok(NULL, " ");
            while (p != NULL) {
                std::cout << p << " ";
                info[i] = atof(p);
                p = strtok(NULL, " ");
                i++;
            }
            std::cout << '\n';
            memcpy(Net::player.pos, &info[0], 3);
            memcpy(Net::player.lookat, &info[3], 3);
            std::cout << Net::player.lookat[1] << "\n";
        }
    }
}
#define LISTENER 1
#define PLAYER 2
int make_socket(int* ID, int type) {
    int sock = 0;
    struct sockaddr_in serv_addr;

    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        printf("\n Socket creation error \n");
        return -1;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    // Convert IPv4 and IPv6 addresses from text to binary form
    if(inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr)<=0)
    {
        printf("\nInvalid address/ Address not supported \n");
        return -1;
    }

    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
        printf("\nConnection Failed \n");
        *ID = 1;
        return -1;
    }
    if(type == LISTENER) {
        send_msg(sock, "listener");
    } else if (type == PLAYER) {
        send_msg(sock, "player");
        char idstr[4];
        recv(sock, idstr, 4, 0);
        *ID = atoi(idstr);
    }
    return sock;
}

#endif
