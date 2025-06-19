# epoll

## Build
```
gcc -o epoll_server main.c
```
## LT mode
```
./epoll_server
```
## ET mode
```
./epoll_server ET
```
## 说明
* `LT`模式：只要fd有数据可读，`epoll_wait`都会返回，适合简单场景。
* `ET`模式：仅在数据到达时触发一次，需一次性读完数据（通过循环），效率更高但需小心处理`EAGAIN`。`ET`模式要求`fd`非阻塞。


## select, poll, epoll区别
* `select`：简单但受限于`fd`数量（1024），每次需要重置`fd_set`，适合小规模连接。
* `poll`：无`fd`数量限制，但仍需遍历所有`fd`，适合中小规模连接。
* `epoll`
  * `LT(Level Trigger)`：水平触发，类似`poll`，易于使用，但可能重复通知。
  * `ET(Edge Trigger)`：边缘触发，效率高但需处理非阻塞`I/O`和一次性读取，适合高并发场景。




