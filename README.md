# DeviceLink

Qt 6 Widgets 练习项目：Windows 上的 **设备调试客户端**，经 **TCP** 或 **串口** 发送/接收 **长度前缀帧**（`uint16` 大端长度 + payload），带彩色日志、解析表和状态栏 RX 计数。

面向面试/自学：信号槽、布局、`QTcpSocket`、`QSerialPort`、`QSettings`。不是完整产品。

## 环境

新手安装见 **[docs/INSTALL-WINDOWS.md](docs/INSTALL-WINDOWS.md)**。

- Windows 10/11
- Qt 6.8+（本仓库用 **6.12 MinGW 64-bit**）+ Qt Creator
- 安装器里除 **MinGW + Qt Creator** 外，还需勾选 **Qt Serial Port**（Maintenance Tool → Add or remove components）
- Python 3 **可选**（仅对照 `tools/echo_server.py`；日常用程序内置 echo 即可）

## 构建

Qt Creator 打开 `CMakeLists.txt`，Kit 选 **Desktop Qt 6.x MinGW 64-bit**，Run。

CMake 模块：`Widgets`、`Network`、`SerialPort`。

已用 Creator 配置过时，命令行示例（路径按本机 `C:\Qt` 调整）：

```bat
cd qt-device-link
cmake --build build\Desktop_Qt_6_12_0_MinGW_64_bit_Debug
```

## 怎么用

### 通道

顶部 **Channel**：

| 模式 | 操作 |
|------|------|
| **TCP** | 填 Host / Port（默认 `127.0.0.1` / `19000`），**Connect** → Send |
| **Serial** | **Refresh Ports** 选 `COMx`，选波特率（默认 115200），**Connect** → Send |

**Connect / Disconnect** 在两种模式下共用；连上后不能改通道和连接参数。

### TCP 自测（无需真设备）

1. 启动后程序会尝试 **自动开内置 echo**（`LocalEchoServer`，日志里应有 `Local echo listening on 127.0.0.1:端口`）。
2. Channel 选 **TCP**，Port 与 echo 一致（通常 **19000**），点 **Connect**。
3. 输入文字 **Send**：日志 TX/RX，表格出一行。
4. 不要用 **9000**（易被占用）。不要与 Python echo **同时**占同一端口。

**Start local echo**：端口改过后可用其 **重新 listen**（与构造里自动启动是同一功能）。

备用：`python tools\echo_server.py`（同样 length-prefix，默认 19000）。

### 串口

- Windows 端口名形如 **`COM3`**，设备管理器可确认。
- 无硬件时：空 COM 或假 `COM99` 可测 **Connect 失败** 与日志 Error。
- 有 USB 转串口：环回（TX 短接 RX）或对端按同帧格式回包，可测 TX/RX/表。
- 协议与 TCP **相同**（2 字节 BE 长度 + payload），`FrameParser` 共用。

### 帧格式

```text
[ byte0: len>>8 ][ byte1: len&0xFF ][ UTF-8 payload ... ]
```

半包/粘包由 `FrameParser` 在 `onBytes` 里处理。

## 代码地图

| 文件 | 干什么 |
|------|------|
| `src/MainWindow.*` | 双通道 UI、信号槽、`QSettings`、日志上色 |
| `src/TcpSession.*` | `QTcpSocket` 客户端 |
| `src/SerialSession.*` | `QSerialPort` 收发 |
| `src/LocalEchoServer.*` | 本机 TCP echo（联调） |
| `src/FrameParser.*` | 长度前缀粘包 |
| `docs/qt-basics.md` | 核心概念 |
| `docs/recap.md` | 面试口述提纲 |
| `PLAN.md` | 原两周任务；串口为 Phase 2 扩展 |

## GitHub

仓库示例：<https://github.com/liliangnike/qt-device-link>

```bat
cd qt-device-link
git add .
git commit -m "Describe your change"
git push
```

不要提交 `build/`（见 `.gitignore`）。可选截图：`docs/screenshot.png` 并在 README 引用。

## 面试 30 秒（中英文思路）

用 Qt 6 Widgets 做了双通道调试工具：**TCP**（`QTcpSocket`）和 **串口**（`QSerialPort`），上层同一套 length-prefix 帧与粘包解析；界面日志、表格、状态栏。收包在 **GUI 线程**，实验室流量可接受；大流量应 `moveToThread` + 信号更新 UI（本仓库未实现）。未做 QML、自定义 TableModel、数据库。