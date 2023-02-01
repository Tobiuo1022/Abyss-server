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

    char *filepath = argv[1];

    char stdin_file[PATHSIZE + 1];
    int stdin_file_len;
    if ((stdin_file_len = readlink("/proc/self/fd/0", stdin_file, sizeof(stdin_file))) < 0) {
        perror("readlink error");
        return 1;
    }
    stdin_file[stdin_file_len] = '\0';

    char stdout_file[PATHSIZE + 1];
    int stdout_file_len;
    if ((stdout_file_len = readlink("/proc/self/fd/1", stdout_file, sizeof(stdout_file))) < 0) {
        perror("readlink error");
        return 1;
    }
    stdout_file[stdout_file_len] = '\0';

    char stderr_file[PATHSIZE + 1];
    int stderr_file_len;
    if ((stderr_file_len = readlink("/proc/self/fd/2", stderr_file, sizeof(stderr_file))) < 0) {
        perror("readlink error");
        return 1;
    }
    stderr_file[stderr_file_len] = '\0';

    char message[strlen(cwd) + 1 + strlen(filepath) + 1 + stdin_file_len + 1 + stdout_file_len + 1 + stderr_file_len + 1];
    char *p = message;
    strncpy(p, cwd, strlen(cwd));
    p += strlen(cwd);
    *p++ = '\n';
    strncpy(p, filepath, strlen(filepath));
    p += strlen(filepath);
    *p++ = '\n';
    strncpy(p, stdin_file, stdin_file_len);
    p += stdin_file_len;
    *p++ = '\n';
    strncpy(p, stdout_file, stdout_file_len);
    p += stdout_file_len;
    *p++ = '\n';
    strncpy(p, stderr_file, stderr_file_len);
    p += stderr_file_len;
    *p++ = '\n';

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
