#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h> 
#include <netinet/ip.h>
#include <netinet/tcp.h>
#include <string.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/time.h>

#include "NetworkHeader.h"

void server_init(int num_clients);
void broadcast_server();

int main (int argc, char** argv) 
{
    if (argc != 2) 
    {
        printf("only one argument is needed\r\n");
        return 0;
    }
    int num_clients = atoi(argv[1]);

    if (num_clients > 0) 
        server_init(num_clients);
    else 
        printf ("Number of clients must be positive\r\n");

    return 0;
}

void broadcast_server() 
{
    int err = 0;

    int sock_bc = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock_bc < 0) perror("socket sock_bc");

    int k = 1;

    err = setsockopt(sock_bc, SOL_SOCKET, SO_BROADCAST, &k, sizeof(int));
    if (err != 0) perror("setsockopt sock_bc broadcast");
    err = setsockopt(sock_bc, SOL_SOCKET, SO_REUSEADDR, &k, sizeof(int));
    if (err != 0) perror("setsockopt sock_bc reuseaddres");

    struct sockaddr_in bc_addr;
    memset(&bc_addr, '0', sizeof(bc_addr));

    bc_addr.sin_family = AF_INET;
    bc_addr.sin_port = htons(CLIENTPORT);
    bc_addr.sin_addr.s_addr = INADDR_BROADCAST;

    err = bind(sock_bc, (struct sockaddr*)&bc_addr, sizeof(bc_addr));
    if (err < 0) perror("bind sock_bc");

    int serv_port = SERVERPORT;
    err = sendto(sock_bc, &serv_port, sizeof(int), 0, (struct sockaddr*)&bc_addr, sizeof(bc_addr));
    if (err < 0) perror("sendto broadcast");
    close(sock_bc);
}

void server_init(int num_clients) 
{
    int err = 0;
    // Non block??
    int sock_connect = socket(AF_INET, SOCK_STREAM, 0);
    if (sock_connect < 0) perror("socket sock_connect");

    struct timeval timeout_accept;
    timeout_accept.tv_sec = ACCEPT_TIMEOUT_SEC;
    timeout_accept.tv_usec = ACCEPT_TIMEOUT_USEC;

    int k = 1;
    err = setsockopt(sock_connect, SOL_SOCKET, SO_REUSEADDR, &k, sizeof(int));
    if (err != 0) perror("setsockopt SO_REUSEADDR");
    err = setsockopt(sock_connect, SOL_SOCKET, SO_RCVTIMEO, &timeout_accept, sizeof(timeout_accept));
    if (err != 0) perror("setsockopt SO_RCVTIMEO");
    err = setsockopt(sock_connect, SOL_SOCKET, SO_KEEPALIVE, &k, sizeof(int));
    if (err != 0) perror("setsockopt SO_KEEPALIVE");

    int keep_cnt = KEEPCNT;
    int keep_idle = KEEPIDLE;
    int keep_intvl = KEEPINTVL;

    err = setsockopt(sock_connect, IPPROTO_TCP, TCP_KEEPCNT, &keep_cnt, sizeof(int));
    if (err != 0) perror("setsockopt keep_cnt");
    err = setsockopt(sock_connect, IPPROTO_TCP, TCP_KEEPIDLE, &keep_idle, sizeof(int));
    if (err != 0) perror("setsockopt keep_idle");
    err = setsockopt(sock_connect, IPPROTO_TCP, TCP_KEEPINTVL, &keep_intvl, sizeof(int));
    if (err != 0) perror("setsockopt keep_intvl");

    struct sockaddr_in serv_addr;
    memset(&serv_addr, '0', sizeof(serv_addr));

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_addr.sin_port = htons(SERVERPORT);

    err = bind(sock_connect, (struct sockaddr*)&serv_addr, sizeof(serv_addr));
    if (err < 0) perror("bind sock_connect");

    err = listen(sock_connect, LISTEN_BACKLOG);
    if (err < 0) perror("listen");

    broadcast_server();

    int* sock_data = (int*)calloc(num_clients, sizeof(int));

    struct sockaddr_in clinet_addrs[num_clients];
    //struct sockaddr_in* clinet_addrs = (struct sockaddr_in*) calloc (num_clients, sizeof(struct sockaddr_in));
    socklen_t client_addr_len = sizeof(clinet_addrs[0]);

    int i = 0;

    for (; i < num_clients; i++) 
    {
        sock_data[i] = accept(sock_connect, NULL, NULL);

        if (sock_data[i] < 0) 
        {

            if (errno == EAGAIN) 
            {
                printf("All clients accepted\n");
                break;
            }
            else perror("accept client");
        }

        struct timeval compute_timeout;
        compute_timeout.tv_sec = COMPUTE_TIMEOUT_SEC;
        compute_timeout.tv_usec = COMPUTE_TIMEOUT_USEC;

        err = setsockopt(sock_data[i], SOL_SOCKET, SO_RCVTIMEO, &compute_timeout, sizeof(compute_timeout));
        if (err != 0) perror ("setsockopt sock_data SO_RCVTIMEO");

        clinet_addrs[i].sin_family = AF_INET;
        clinet_addrs[i].sin_port = htons(CLIENTPORT);
    }

    int connected_clients = i;

    if (connected_clients == 0) 
    {
        printf("No clients connected\n");
        close(sock_connect);
        return;
    }

    printf("%d clients connected\n", i);

    struct timeval time_begin, time_end;
    gettimeofday(&time_begin, 0);

    int global_threads = 0;

    int* client_threads = (int*)calloc(connected_clients, sizeof(int));

    for (int i = 0; i < connected_clients; ++i) 
    {
        int ret = recv(sock_data[i], &(client_threads[i]), sizeof(int), 0);

        if (ret < 0) perror("recv client_thread");

        if (ret != sizeof(int)) 
        {
            printf("client disconnect\n");

            for (int i = 0; i < connected_clients; i++) close(sock_data[i]);

            close(sock_connect);

            return;
        }

        global_threads += client_threads[i];
    }

    printf ("global_threads: %d\n", global_threads);

    double global_start = -10;
    double global_fin = 10;
    double global_delta = 0.000000025;

    for (int i = 0; i < connected_clients; ++i) {

        struct boards_info general_boards;
        general_boards.left = global_start;
        general_boards.right = global_start;

        double interval = (global_fin - global_start) / global_threads;

        for (int j = 0; j < i; j++) {

            general_boards.left += interval * client_threads[j];
            general_boards.right += interval * client_threads[j];
        }
        general_boards.right += interval * client_threads[i];
        general_boards.delta = global_delta;

        err = sendto(sock_data[i], &general_boards, sizeof(general_boards), 0, (struct sockaddr*)(&clinet_addrs[i]), client_addr_len);

        if (err != sizeof(general_boards)) perror("sendto general_boards");
    }

    free (client_threads);
    double sum_global = 0;

    for (int i = 0; i < connected_clients; i++) {

        printf("client_addr = %s\n", inet_ntoa(clinet_addrs[i].sin_addr));

        double res;

        int ret = recv(sock_data[i], &res, sizeof(double), 0);

        if (ret != sizeof(double)) 
        {
            printf("client disconnect\n");

            for (int i = 0; i < connected_clients; i++)
                close(sock_data[i]);

            close(sock_connect);

            return;
        }

        sum_global += res;
        printf("res[%d]: %lf\n", i, res);

        close (sock_data[i]);
    }

    free(sock_data);

    gettimeofday (&time_end, 0);
    int seconds = time_end.tv_sec - time_begin.tv_sec;
    int microseconds = time_end.tv_usec - time_begin.tv_usec;
    double sum_time = seconds + (double)microseconds / 1000 / 1000;

    printf ("The result is: %lf\nReal time is: %.3f seconds\n", sum_global, sum_time);

    close(sock_connect);
}