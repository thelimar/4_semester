#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <arpa/inet.h>

#include "NetworkHeader.h"

void client_init(int num_threads);
double compute_integral(int num_threads, struct boards_info general_boards);

int main(int argc, char** argv)
{
    if (argc != 2) 
    {
        printf("Only one argument is needed\r\n");
        return 0;
    }

    int num_threads = atoi(argv[1]);
    if (num_threads > 0) while (1) client_init(num_threads);
    else printf("Number of clients must be positive\r\n");

    return 0;
}

void broadcast_client(int* serv_port, struct sockaddr_in* peer_addr, socklen_t* peer_addr_len) {

    int err = 0;

    int sock_bc = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock_bc < 0) perror("socket sock_bc");

    int k = 1; // need > 0

    struct sockaddr_in local_addr;
    memset(&local_addr, '0', sizeof(local_addr));

    local_addr.sin_family = AF_INET;
    local_addr.sin_port = htons(CLIENTPORT);
    local_addr.sin_addr.s_addr = htonl(INADDR_ANY);

    err = setsockopt(sock_bc, SOL_SOCKET, SO_REUSEADDR, &k, sizeof(int));
    if (err != 0) perror("setsockopt sock_bc");

    err = bind(sock_bc, (struct sockaddr*)&local_addr, sizeof(local_addr));
    if (err < 0) perror("bind sock_bc");

    printf("Waiting for server...\n");

    err = recvfrom(sock_bc, serv_port, sizeof(int), 0, (struct sockaddr*)peer_addr, peer_addr_len);
    if (err != sizeof(int)) perror("recvfrom serv_port");

    if (*serv_port != SERVERPORT) return;

    close(sock_bc);
}

int tcp_client(int* serv_port, struct sockaddr_in* peer_addr, int num_threads) {

    int err = 0;

    int sock_data = socket(AF_INET, SOCK_STREAM, 0);

    int keep_cnt = KEEPCNT;
    int keep_idle = KEEPIDLE;
    int keep_intvl = KEEPINTVL;

    err = setsockopt(sock_data, IPPROTO_TCP, TCP_KEEPCNT, &keep_cnt, sizeof(int));
    if (err != 0) perror("setsockopt keep_cnt");
    err = setsockopt(sock_data, IPPROTO_TCP, TCP_KEEPIDLE, &keep_idle, sizeof(int));
    if (err != 0) perror("setsockopt keep_idle");
    err = setsockopt(sock_data, IPPROTO_TCP, TCP_KEEPINTVL, &keep_intvl, sizeof(int));
    if (err != 0) perror("setsockopt keep_intvl");

    struct sockaddr_in serv_addr;
    memset(&serv_addr, '0', sizeof(serv_addr));

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(*serv_port);
    serv_addr.sin_addr = peer_addr->sin_addr;

    socklen_t serv_addr_len = sizeof(serv_addr);

    err = connect(sock_data, (struct sockaddr*)&serv_addr, serv_addr_len);
    if (err < 0) perror("sock_data connect");

    printf("Connected!\n");

    return sock_data;
}

void client_compute(int sock_data, int num_threads) {

    int err = 0;

    int threads = num_threads;
    err = send(sock_data, &threads, sizeof(int), MSG_NOSIGNAL);
    if (err < 0) perror("send num_threads");

    struct boards_info general_boards;
    memset(&general_boards, '0', sizeof(general_boards));
    err = recv(sock_data, &general_boards, sizeof(general_boards), 0);
    if (err < 0) perror("recieve general_boards info");

    printf("left = %lf, right = %lf\n", general_boards.left, general_boards.right);

    double res = compute_integral(num_threads, general_boards);

    printf("The result is: %lf\n", res);

    err = send(sock_data, &res, sizeof(double), MSG_NOSIGNAL);
    if (err < 0) perror("send result");

    close(sock_data);

    printf("Client: all work is done\n");
    return;
}

void client_init(int num_threads) 
{
    int serv_port = 0;

    struct sockaddr_in peer_addr;
    memset(&peer_addr, '0', sizeof(peer_addr));
    socklen_t peer_addr_len = sizeof(peer_addr);

    broadcast_client(&serv_port, &peer_addr, &peer_addr_len);
    int sock_data = tcp_client(&serv_port, &peer_addr, num_threads);
    client_compute(sock_data, num_threads);
}