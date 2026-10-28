# DeviceLink

两周 Qt Widgets 面试小项目：Windows 上位机 TCP 客户端，发送/接收 **长度前缀帧**（`uint16` 大端长度 + payload），日志 + 表格。

和工控「Qt + 连设备」岗能对上话；不是完整产品。

## 环境

新手请先读并做完：**[docs/INSTALL-WINDOWS.md](docs/INSTALL-WINDOWS.md)**（下载安装 Qt 6、MinGW、Creator、Python、第一次编译运行）。

- Windows 10/11
- Qt 6.8 或 6.9 + Qt Creator（安装器里勾 **MinGW 64-bit**，不必先装 Visual Studio）
- Python 3 可选（备用 echo 服务器；安装时勾 Add to PATH）

## 构建（命令行示例）

把路径换成你的 Qt 安装位置：

```bat
cmake -S . -B build -DCMAKE_PREFIX_PATH=C:\Qt\6.8.3\msvc2022_64
cmake --build build --config Release
```

## 怎么玩

启动后默认 `127.0.0.1:19000`，并自动开本机 echo。点 **Connect** → 输入文字回车。

发送格式：2 字节大端长度 + UTF-8 文本。echo 原样送回，下方表格应出现一行。

不要用 **9000**（本机常被别的程序占用）。也不要同时开 Python echo 和窗口里的 **Start local echo**。

备用（可选）：先不要点 Start local echo，另开终端：

```bat
python tools\echo_server.py
```

脚本也监听 `127.0.0.1:19000`。Windows 上 `Ctrl+C` 若退不出，在任务管理器结束 `python.exe`。

## 代码地图

| 文件 | 面试时说 |
|---|---|
| `src/MainWindow.*` | 布局、菜单、信号槽、QSettings |
| `src/TcpSession.*` | `QTcpSocket` 异步收发 |
| `src/LocalEchoServer.*` | 本机 `127.0.0.1:19000` echo |
| `src/FrameParser.*` | 粘包：长度前缀 |
| `docs/qt-basics.md` | 核心知识点 |
| `PLAN.md` | 14 天任务 |

## 上传到自己的 GitHub

在本目录：

```bat
cd qt-device-link
git init
git add .
git commit -m "Add DeviceLink Qt interview lab"
gh repo create DeviceLink --public --source=. --remote=origin --push
```

没有 `gh` 就在 GitHub 网页新建空仓库，再 `git remote add origin <url>` 后 `git push -u origin main`。

不要提交 `build/`（已在 `.gitignore`）。README 里换成你的用户名和截图。

## 面试怎么说（30 秒）

Windows 上用 Qt 6 Widgets 做了设备调试客户端：信号槽连 TCP，自己解析长度前缀帧（和嵌入式协议半包同一类问题）。日常本职是 ONT 上 Linux 应用层；这个仓库补的是上位机 Qt 基础。
