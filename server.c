#include <sys/socket.h>
#include <sys/types.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <stdarg.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/time.h>
#include <sys/ioctl.h>
#include <netdb.h>

#define SERVER_PORT 18000
#define MAX_LINE 4096
#define SA struct sockaddr

void err_n_die (const char *fmt, ...);

int main(int argc, char **argv)
{
    int listenfd, connfd, n;
    struct sockaddr_in servaddr;
    uint8_t buff[MAX_LINE+1];
    uint8_t recvline[MAX_LINE+1];

    if ((listenfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
        err_n_die("socket error");

    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(SERVER_PORT);

    if (bind(listenfd, (SA *)&servaddr, sizeof(servaddr)) < 0)
        err_n_die("bind error");

    if (listen(listenfd, 10) < 0)
        err_n_die("listen error");

    for (;;) {
        printf("Waiting for connection on port %d\n", SERVER_PORT);

        connfd = accept(listenfd, NULL, NULL);
        if (connfd < 0)
            err_n_die("accept error");

        while ((n = read(connfd, recvline, MAX_LINE-1)) > 0) {
            recvline[n] = 0;
            printf("%s\n", recvline);

            if (recvline[n-1] == '\n')
                break;
        }

        snprintf((char*)buff, sizeof(buff),
                 "HTTP/1.0 200 OK\r\n"
                 "Content-Type: text/plain\r\n"
                 "\r\n"
                 "Hello\n");

        write(connfd, buff, strlen((char*)buff));
        close(connfd);
    }
}

void err_n_die(const char *fmt, ...)
{
    int errno_save = errno;
    va_list ap;

    va_start(ap, fmt);
    vfprintf(stdout, fmt, ap);
    fprintf(stdout, "\n");

    if (errno_save != 0)
        fprintf(stdout, "(errno = %d) : %s\n",
                errno_save, strerror(errno_save));

    va_end(ap);
    exit(1);
}