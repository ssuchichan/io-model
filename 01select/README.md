# select
`select`监控标准输入和服务器套接字，每次循环都需要重置`fd_set`。性能受限于`fd`数量和`O(n)`遍历。