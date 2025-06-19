#include <stdio.h>
#include <stdlib.h>
#include <poll.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <string.h>

#define MAX_FDS 10


int main(void) {
    int server_fd, client_fd;
    struct sockaddr_in server_addr, client_addr;
    socklen_t addr_len = sizeof(client_addr);
    struct pollfd fds[MAX_FDS];
    int nfds = 1;

    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(8080);

    bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr));
    listen(server_fd, 5);
    printf("Server listening on port 8080...\n");

    fds[0].fd = server_fd;
    fds[0].events = POLLIN;

    while (1) {
        // poll
        if (poll(fds, nfds, -1) < 0) {
            perror("poll");
            exit(-1);
        }

        // check every fd
        for (int i = 0; i < nfds; i++) {
            if (fds[i].fd == server_fd) {
                // new connection
                client_fd = accept(server_fd, (struct sockaddr *)&client_addr, &addr_len);
                if (nfds < MAX_FDS) {
                    fds[nfds].fd = client_fd;
                    fds[nfds].events = POLLIN;
                    nfds++;
                    printf("New client connected: fd %d\n", client_fd);
                }
            } else {
                // client data
                char buf[1024] = {0};
                int n = read(fds[i].fd, buf, sizeof(buf));
                if (n <= 0) {
                    close(fds[i].fd);
                    fds[i] = fds[nfds - 1];
                    nfds--;
                    printf("Client disconnected: fd %d\n", fds[i].fd);
                } else {
                    buf[n] = '\0';
                    printf("Client %d: %s", fds[i].fd, buf);
                }
            }
        }

    }

    close(server_fd);
    return 0;
}