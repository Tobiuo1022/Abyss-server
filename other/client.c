#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
 
#define PORT_NUM 3333
#define HOSTNAME "127.0.0.1"
#define BUFSIZE 4096
#define PATHSIZE 4096

int main(int argc, char *argv[]) {
    // Setup server socket.
    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT_NUM);
    server_addr.sin_addr.s_addr = inet_addr(HOSTNAME);
   
    int socket_fd;
    // Make a socket.
    if ((socket_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("socket error");
        return 1;
    }
   
    // Connect server.
    if (connect(socket_fd, (struct sockaddr *) &server_addr, sizeof(server_addr)) < 0) {
        perror("connect error");
        if (close(socket_fd) < 0) {
            perror("close");
        }
        return 1;
    }

    // make the message. 
    char cwd[PATHSIZE];
    getcwd(cwd, sizeof cwd);

    char *tty;
    if ((tty = ttyname(1)) == NULL) {
        perror("ttyname error");
        return 1;
    }

    char *filepath = argv[1];

    char message[strlen(cwd) + 1 + strlen(tty) + 1 + strlen(argv[1]) + 1];
    char *p = message;
    strncpy(p, cwd, strlen(cwd));
    p += strlen(cwd);
    *p++ = '\n';
    strncpy(p, tty, strlen(tty));
    p += strlen(tty);
    *p++ = '\n';
    strncpy(p, filepath, strlen(filepath));
    p += strlen(filepath);
    *p++ = 0;

    // Send a message.
    int send_len;
    if ((send_len = send(socket_fd, message, p-message, 0)) < 0) {
        perror("send error");
        if (close(socket_fd) < 0) {
            perror("close error");
        }
        return 1;
    }

    // Receive a done message.
    char *buf = (char*)calloc(BUFSIZE, sizeof(char));
    int read_len;
    if ((read_len = read(socket_fd, buf, sizeof(buf))) < 0) {
        fprintf(stderr, "read error\n");
    }
    free(buf);

    // close a socket. 
    if (close(socket_fd) < 0) {
        perror("close error");
        return 1;
    }
   
    return 0;
}
