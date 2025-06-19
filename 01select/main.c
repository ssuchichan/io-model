#include <stdio.h>
#include <stdlib.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <string.h>


int main(void) {
    int server_fd, client_fd;
    struct sockaddr_in server_addr, client_addr;
    socklen_t addr_len = sizeof(client_addr);
    fd_set read_fds;

    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(8080);

    bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr));
    listen(server_fd, 5);
    printf("Server listening on port 8080...\n");

    while (1) {
        FD_ZERO(&read_fds);
        FD_SET(STDIN_FILENO, &read_fds);
        FD_SET(server_fd, &read_fds);
        int max_fd = server_fd > STDIN_FILENO ? server_fd : STDIN_FILENO;

        // select
        if (select(max_fd + 1, &read_fds, NULL, NULL, NULL) < 0) {
            perror("select");
            exit(1);
        }

        // check stdin
        if (FD_ISSET(STDIN_FILENO, &read_fds)) {
            char buf[1024] = {0};
            fgets(buf, sizeof(buf), stdin);
            printf("STDIN: %s", buf);
        }

        // check new connection
        if (FD_ISSET(server_fd, &read_fds)) {
            client_fd = accept(server_fd, (struct sockaddr *)&client_addr, &addr_len);
            char buf[1024] = {0};
            int n = read(client_fd, buf, sizeof(buf));
            buf[n] = '\0';
            printf("Client: %s", buf);
            close(client_fd);
        }
    }
    close(server_fd);
    return 0;
}