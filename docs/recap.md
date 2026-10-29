# 对着仓库口头总结（第 14 天用）

## 30 秒

用 Qt 6 在 Windows 上做了一个 TCP 调试工具。界面是 Widgets：连接、发送、日志、解析表。协议是 2 字节大端长度加 payload，自己处理了 TCP 粘包。两周练的是信号槽、布局、`QTcpSocket` 和简单组帧。

## 可能被问到

**为什么不 QML？**  
桌面调试工具主流还是 Widgets。两周目标是信号槽和布局，QML 以后再说。

**QTableWidget 和 Model/View？**  
表很小，用 `QTableWidget`。行数到几千应改 `QAbstractTableModel`，避免每个 cell 一个 `QTableWidgetItem`。

**会阻塞界面吗？**  
`QTcpSocket` 异步，收包在 GUI 线程只做 append 和解析，数据量小没问题。大流量应独立线程，用信号把结果送回界面。

**和设备上的协议有什么关系？**  
设备里跑的是协议栈或守护进程；DeviceLink 是电脑上的调试客户端。帧格式、半包、会话是同一类问题，平台不同。

**不会什么？**  
没做串口、QML、自定义 delegate、数据库。需要持久化配置时，本工具这种体量用 `QSettings` 或 SQLite 即可。
