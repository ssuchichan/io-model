# poll
`poll`使用`pollfd`数组管理`fd`，无数量限制，但仍需遍历所有`fd`，性能`O(n)`。