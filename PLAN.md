# 两周计划（每天 60–90 分钟）

目标：能运行 DeviceLink、能讲清信号槽/布局/TCP/粘包，GitHub 上有 README 和截图。不是成为 Qt 专家。

环境与安装逐步说明见 **[`docs/INSTALL-WINDOWS.md`](docs/INSTALL-WINDOWS.md)**（账号、下载、勾选组件、Creator 打开工程、Python、第一次 Connect）。新手从 Day 1 起严格按那份文档做，不要跳步。

每天：先看 `docs/qt-basics.md` 对应节，再改/跑代码。

---

## 第 1 周 — 能跑、能讲控件

**Day 1 安装（按 INSTALL 逐步勾）**  
打开 [`docs/INSTALL-WINDOWS.md`](docs/INSTALL-WINDOWS.md)，按第 1～6 节做完：注册 Qt 账号 → 下载安装器 → 勾选 **Qt 6.x + MinGW 64-bit + Qt Creator + CMake + Ninja** → 打开本目录的 `CMakeLists.txt` → Run 弹出主窗口。  
有余力做第 7 节：Connect `127.0.0.1:19000` + 发送 `hello`（程序会自动开本机 echo）。  
完成标准以 INSTALL 第 9 节清单为准。不要用 MSVC Kit（除非你已经装了 VS 2022）。

**Day 2 对象树**  
读 `main.cpp`、`MainWindow` 构造。改窗口标题、默认尺寸。口述：`parent` 是谁、关掉窗口内存谁释放。

**Day 3 信号槽**  
读 `TcpSession` 里 `connect(...)`。自己给「Clear log」加一个工具栏按钮（`QToolBar`），接到已有 `onClearLog`。

**Day 4 布局**  
读 `buildUi()`。把 Host/Port 那一行改成 `QFormLayout` 或加一个 stretch。拉大窗口，看谁跟着变。

**Day 5 QMainWindow**  
菜单已经有。再加 **Help → Qt basics** 打开 `docs/qt-basics.md` 或弹 `QMessageBox` 三句话（信号槽、parent、事件循环）。

**Day 6 TCP**  
确认 Port 是 **19000**（不要用 9000），日志有 `Local echo listening on 127.0.0.1:19000`，再 **Connect**，发 `hello`。对照 `TcpSession.cpp` 讲 `connected` / `readyRead`。

**Day 7 日志**  
给日志加颜色或前缀 TX/RX（可用 `appendHtml` 或保持纯文本加 `[TX]`）。Git 提交：`day7: tcp echo works`。

---

## 第 2 周 — 协议、收尾、GitHub

**Day 8 粘包**  
读 `FrameParser`。用 Wireshark 不必。口述：为什么先收 2 字节长度。故意只发半帧（可改 echo 服务器）看表会不会等到收齐。

**Day 9 表格**  
给表加第 4 列 Hex。`payload.toHex(' ')`。这是 `QTableWidget`；被问 Model/View 按笔记回答。

**Day 10 不要卡界面**  
在 About 里写一句：本工具收包在 GUI 线程，数据量小可以。面试加分：大流量应 `moveToThread`。可选：Connect 时 `QApplication::setOverrideCursor`。

**Day 11 设置与状态栏**  
确认关掉再开会记住 host/port（`QSettings`）。状态栏显示已收字节数（自己加一个 `m_rxBytes`）。

**Day 12 打磨**  
空 host 点 Connect 要 `QMessageBox::warning`。断开后清空 parser。检查没有内存乱 `delete`。

**Day 13 GitHub**  
见 README「上传」。截一张主窗口图放到 `docs/screenshot.png`。写清：如何安装 Qt、如何跑 echo、帧格式。

**Day 14 面试口述（对着仓库 5 分钟）**  
1. 这是什么：设备 TCP 调试上位机。  
2. 信号槽连了哪几条。  
3. 帧格式和粘包。  
4. 和 ONT 的关系：设备侧协议 vs 这边是 Windows 调试工具。  
5. 不会的：QML、自定义 Model、串口（没做）。诚实。

---

每日不要超过 90 分钟。有毕勤/埃地沃兹约面，当天改准备那一场，Qt 停一天无妨。
