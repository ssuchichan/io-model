#include <stdio.h>
#include <stdlib.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>

#define MAX_EVENTS 1024
#define PORT 8080

void set_nonblocking(int fd) {
    int flags = fcntl(fd, F_GETFD, 0);
    fcntl(fd, F_SETFL, flags | O_NONBLOCK);
}

int main(int argc, char *argv[]) {
    int server_fd, client_fd, epoll_fd;
    struct sockaddr_in server_addr, client_addr;
    socklen_t addr_len = sizeof(client_addr);
    struct epoll_event event, events[MAX_EVENTS];
    int use_et = (argc > 1 && strcmp(argv[1], "ET") == 0);

    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr));
    listen(server_fd, 5);
    set_nonblocking(server_fd);
    printf("Server listening on port %d (%s mode)...\n", PORT, use_et ? "ET" : "LT");

    epoll_fd = epoll_create1(0);
    event.data.fd = server_fd;
    event.events = EPOLLIN | (use_et ? EPOLLET : 0);
    epoll_ctl(epoll_fd, EPOLL_CTL_ADD, server_fd, &event);

    while (1) {
        int n = epoll_wait(epoll_fd, events, MAX_EVENTS, -1);
        for (int i = 0; i < n; i++) {
            if (events[i].data.fd == server_fd) {
                client_fd = accept(server_fd, (struct sockaddr *)&client_addr, &addr_len);
                set_nonblocking(client_fd);
                event.data.fd = client_fd;
                event.events = EPOLLIN | (use_et ? EPOLLET : 0);
                epoll_ctl(epoll_fd, EPOLL_CTL_ADD, client_fd, &event);
                printf("New client connected: fd %d\n", client_fd);
            } else {
                char buf[1024] = {0};
                int fd = events[i].data.fd;
                if (use_et) {
                    // ET mode: read all data at once
                    while (1) {
                        int nread = read(fd, buf, sizeof(buf));
                        if (nread <= 0) {
                            if (nread == 0 || (nread < 0 && errno != EAGAIN)) {
                                close(fd);
                                printf("Client %d disconnected\n", fd);
                                break;
                            }
                            break; // EAGAIN: no data
                        }
                        buf[nread] = '\0';
                        printf("Client %d: %s", fd, buf);
                    }
                } else {
                    // LT mode: read normally
                    int nread = read(fd, buf, sizeof(buf));
                    if (nread <= 0) {
                        close(fd);
                        printf("Client %d disconnected\n", fd);
                    } else {
                        buf[nread] = '\0';
                        printf("Client %d: %s", fd, buf);
                    }
                }
            }
        }
    }
    close(server_fd);
    close(epoll_fd);
    return 0;
}