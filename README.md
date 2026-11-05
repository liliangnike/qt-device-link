# DeviceLink

两周 Qt Widgets 速成实验：Windows 上做一个 TCP 调试客户端，发送/接收 **长度前缀帧**（`uint16` 大端长度 + payload），带日志和表格。

面向零基础或只会 C 的读者；按 `PLAN.md` 每天 60–90 分钟即可跑通。不是完整产品。

## 环境

新手请先读并做完：**[docs/INSTALL-WINDOWS.md](docs/INSTALL-WINDOWS.md)**（下载安装 Qt 6、MinGW、Creator、Python、第一次编译运行）。

- Windows 10/11
- Qt 6.8 或 6.9 + Qt Creator（安装器里勾 **MinGW 64-bit**，不必先装 Visual Studio）
- Python 3 可选（备用 echo 服务器；安装时勾 Add to PATH）

## 构建

日常用 Qt Creator 打开 `CMakeLists.txt`，Kit 选 **MinGW 64-bit**，点 Run。

命令行不要 `cmake --build build`（Creator 的缓存不在 `build` 根下）。已用 Creator 配过的话：

```bat
cd qt-device-link
cmake --build build\Desktop_Qt_6_12_0_MinGW_64_bit_Debug
```

自己另建目录（版本号改成你 `C:\Qt` 下实际文件夹）：

```bat
set PATH=C:\Qt\Tools\mingw1310_64\bin;C:\Qt\6.12.0\mingw_64\bin;%PATH%
cmake -S . -B build-cli -G Ninja -DCMAKE_PREFIX_PATH=C:\Qt\6.12.0\mingw_64
cmake --build build-cli
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

| 文件 | 干什么 |
|---|---|
| `src/MainWindow.*` | 布局、菜单、信号槽、QSettings |
| `src/TcpSession.*` | `QTcpSocket` 异步收发 |
| `src/LocalEchoServer.*` | 本机 `127.0.0.1:19000` echo |
| `src/FrameParser.*` | 粘包：长度前缀 |
| `docs/qt-basics.md` | 核心知识点 |
| `docs/recap.md` | 学完对着仓库口头总结 |
| `PLAN.md` | 14 天任务 |

## 上传到自己的 GitHub

在本目录：

```bat
cd qt-device-link
git init
git add .
git commit -m "Add DeviceLink Qt Widgets lab"
gh repo create DeviceLink --public --source=. --remote=origin --push
```

没有 `gh` 就在 GitHub 网页新建空仓库，再 `git remote add origin <url>` 后 `git push -u origin main`。

不要提交 `build/`（已在 `.gitignore`）。README 里换成你的用户名和截图。

## 学完能讲清什么（30 秒）

用 Qt 6 Widgets 做了一个 TCP 调试客户端：信号槽连接网络，自己解析长度前缀帧（处理粘包/半包），界面有日志和表格。两周目标是会跑、能讲清对象树、布局和 `QTcpSocket`，不是成为 Qt 专家。
