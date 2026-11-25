# 对着仓库口头总结（面试用）

## 30 秒

用 Qt 6 Widgets 在 Windows 上做了 **DeviceLink**：**TCP** 或 **串口** 调试客户端。界面选 Channel，连接后发 length-prefix 帧（2 字节大端 + payload），`FrameParser` 处理粘包，日志着色 + 表格 + 状态栏 RX。TCP 自测用内置 **LocalEchoServer**（127.0.0.1:19000）。练了信号槽、布局、`QTcpSocket`、`QSerialPort`、`QSettings`。

## 可能被问到

**为什么不 QML？**  
工控/调试类桌面工具常见 Widgets。本项目标是信号槽和网络/串口，QML 未做。

**TCP 和串口怎么共用逻辑？**  
`TcpSession` / `SerialSession` 都 `emit bytesReceived`，`MainWindow::onBytes` 同一套；发送按 Channel 分支 `sendBytes`。

**QTableWidget 和 Model/View？**  
表很小用 `QTableWidget`；大数据应用 `QAbstractTableModel` + `QTableView`。

**会阻塞界面吗？**  
`readyRead` 在 GUI 线程里解析、刷日志，小流量可以。大流量应 worker 线程 + 信号回 UI（本仓库未做 `moveToThread`）。

**和设备协议的关系？**  
板端跑协议栈；这是 PC 侧调试工具。半包、组帧和设备侧是同一类问题。

**不会什么 / 诚实边界**  
QML、自定义 Model、数据库、Modbus、`moveToThread` 实装。串口已做基础 open/read/write，未做流控/RS485 方向脚等工控细节。