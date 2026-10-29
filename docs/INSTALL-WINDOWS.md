# Windows 安装与第一次运行（新手逐步做）

本课用 **Qt 6 + Qt Creator + MinGW**。不必先装 Visual Studio。全部做完大约 40–70 分钟（含下载）。

你要得到三样东西：

1. Qt Creator（写代码、点按钮编译）
2. Qt 6 库（MinGW 64 位）
3. Python 3（可选；本课优先用程序内 **Start local echo**，不必跑脚本）

---

## 0. 开始前

- 系统：Windows 10 或 11，磁盘至少空余 **15 GB**（安装包和解压都占空间）。
- 用有线或稳定 Wi‑Fi。安装器要联网拉组件。
- 路径尽量用默认 `C:\Qt`，**不要**装到含中文或空格过多的目录。
- 下面按钮名称以官方在线安装器为准，版本不同可能略有出入，勾选原则不变。

---

## 1. 注册免费 Qt 账号

1. 打开：<https://login.qt.io/login>
2. 点 **Sign up** 注册（邮箱即可）。
3. 邮箱点确认链接。后面安装器登录用这个账号。
4. 选开源（Open Source / GPL/LGPL），个人学习免费。不要买商业版。

---

## 2. 下载在线安装器

1. 打开：<https://www.qt.io/download-qt-installer>
2. 若页面让你选商业/开源，选 **开源 / Go open source**。
3. 下载 **Qt Online Installer for Windows**（文件名类似 `qt-online-installer-windows-x64-*.exe`）。
4. 下完后双击运行。若 SmartScreen 提示「未识别的应用」，选 **更多信息 → 仍要运行**。

---

## 3. 安装向导里怎么勾

逐步点 **Next**，关键页如下。

### 3.1 登录

用第 1 步的 Qt 账号登录。

### 3.2 欢迎 / 贡献

可勾可不勾「发送统计」。Next。

### 3.3 安装路径

建议：

```text
C:\Qt
```

记住这个路径。Next。

### 3.4 选择组件（最重要）

选 **Custom / 自定义**，不要只装默认精简包（可能没有 MinGW）。

在树里勾选下面这些（名称随小版本略变，对上意思即可）：

**A. Qt 6.8.x 或 6.9.x**（列表里最新的 6.8/6.9 都行）

在该版本下勾选：

- **MinGW 13.x 64-bit**（或 MinGW 11/13 64-bit，有哪个勾哪个）
- 不要勾：Qt Charts、Qt Quick 3D、Android、Sources、Debug Information Files（体积大，本课用不到）
- **Qt 5 Compatibility Module** 不必勾

> Network（`Qt6::Network`）在 Qt 主模块里，一般不用单独勾「Qt Network」。

**B. Developer and Designer Tools（开发者工具）**

勾选：

- **Qt Creator**（C++）
- **CMake**
- **Ninja**
- **MinGW 13.x 64-bit**（和上面 Qt 那一项版本一致，安装器里常出现两次，两处都勾）

**不要勾：** Debug symbols、整个 Qt 5.15 老版本（除非你知道自己要）。

勾完点 Next。体积大约数 GB，耐心等。

### 3.5 许可协议

选开源许可，勾同意。Next → Install。  
进度条走完点 Finish。可勾 **Launch Qt Creator**。

---

## 4. 第一次打开 Qt Creator

1. 开始菜单搜 **Qt Creator** 打开。
2. 菜单 **Edit → Preferences**（有的版本是 **Tools → Options**）。
3. 左侧 **Kits（构建套件）**。
4. 应看到类似：

   `Desktop Qt 6.8.3 MinGW 64-bit`

   左边是绿色或黄色都可以用。若是 **红色**：

   - 回到安装器（开始菜单 **Qt Maintenance Tool**），点 **Add/Remove**，补勾 MinGW 和对应 Qt 6。
   - 或重启 Creator 后再看 Kits。

5. 先不要改编译器路径。Apply → OK。

---

## 5. 安装 Python 3（可选）

本课联调用程序内的 **Start local echo**（`127.0.0.1:19000`）即可。Python 只在你想对照 `tools/echo_server.py` 时才需要。

1. 打开：<https://www.python.org/downloads/windows/>
2. 下载 **Windows installer (64-bit)**。
3. 安装第一屏 **务必勾选** `Add python.exe to PATH`。
4. 选 Install Now。
5. 关掉所有已打开的终端，新开一个 **PowerShell** 或 **cmd**，输入：

```bat
python --version
```

应显示 `Python 3.x.x`。若提示找不到命令：关掉终端重开；仍不行就把安装器修好，勾 PATH 再装一次。

---

## 6. 用 Qt Creator 打开 DeviceLink

1. 确认已经打开 **本仓库根目录**（里面有 `CMakeLists.txt`、`src\`、`docs\`）。
2. Qt Creator：**File → Open File or Project…**
3. 选这个文件（不要选某一个 `.cpp`）：

   `（仓库根目录）\CMakeLists.txt`

4. 弹出 Configure Project：
   - Kit 选 **Desktop Qt 6.x MinGW 64-bit**
   - 构建目录可用默认 `...\qt-device-link\build\...`
   - 点 **Configure Project**
5. 左下角模式选 **Debug**（虫子图标旁的构建套件）。
6. 左下角点 **绿色三角 Run**（或 `Ctrl+R`）。
7. 第一次会编译 1～2 分钟。成功则弹出窗口，标题类似 `DeviceLink`。
8. 窗口 Port 应是 **19000**，日志可出现 `Local echo listening on 127.0.0.1:19000`。能看到 Host / Port / 日志框即 Day 1 过关。

### 6.1 常见失败

| 现象 | 处理 |
|---|---|
| CMake 报 `Could not find Qt6` | Kit 选错成 MSVC 但没装 VS。改用 **MinGW** 那个 Kit，重新 Configure。 |
| `No CMake tool` | Maintenance Tool 补装 CMake、Ninja，重启 Creator。 |
| 编译一堆 `Q_OBJECT` / moc | 确认 `CMakeLists.txt` 里有 `CMAKE_AUTOMOC ON`，且 `MainWindow.h` 在可执行文件源列表里（仓库已写好）。 |
| 窗口一闪没有 | 看 Creator 下方 **Application Output** 红字。 |
| 杀毒拦截 `DeviceLink.exe` | 允许一次即可。 |

改完代码后：`Ctrl+B` 构建，`Ctrl+R` 运行。不要手动去 `build` 文件夹里找 exe，用 Creator 的 Run。

---

## 7. 第一次联调（可放在 Day 1 末尾或 Day 6）

**推荐（不需要 Python）：**

1. Host 保持 `127.0.0.1`，Port 必须是 **19000**（不要填 9000）。
2. 启动时已自动开本机 echo；日志应有 `Local echo listening on 127.0.0.1:19000`。若没有，再点 **Start local echo**。
3. 点 **Connect** → 输入 `hello` 回车。
4. 日志应有 TX、Local echo: bounced、RX，下方表格多一行。

若日志仍是 `Connecting to 127.0.0.1:9000`：跑的是旧 exe，在 Qt Creator 里 **Build → Rebuild All** 后再 Run。  
若 Connect 成功但一发送就断开：端口不是 19000，或 19000 上另有程序（例如卡住的 `echo_server.py`）。

**可选（Python 服务器，端口同样是 19000）：**

先不要点 Start local echo，也不要让程序自动占用 19000。另开终端：

```bat
cd （仓库根目录）
python tools\echo_server.py
```

看到 `echo server on 127.0.0.1:19000` 后，窗口 Port 填 **19000**，只点 Connect。不要和内置 echo 同时开。

Windows 防火墙若弹出「允许 Python 访问网络」，勾选专用网络，允许。只监听 127.0.0.1，一般可以。

停服务器：在 PowerShell 里 `Ctrl+C`。若退不出，任务管理器结束 `python.exe`。

---

## 8. 命令行编译（可选，不熟练可跳过）

新手用 Creator 即可。命令行能编，但 **不要** 对空的 `build` 目录直接 `cmake --build build`。

Qt Creator 真正的构建目录是套件子文件夹，例如：

```bat
cd （仓库根目录）
cmake --build build\Desktop_Qt_6_12_0_MinGW_64_bit_Debug
```

`Error: could not load cache` 表示这个目录里没有 `CMakeCache.txt`（Creator 没 Configure 过，或路径写短了，写成了 `build`）。

若要自己新建一份、不碰 Creator 的目录：

```bat
cd （仓库根目录）
set PATH=C:\Qt\Tools\mingw1310_64\bin;C:\Qt\6.12.0\mingw_64\bin;%PATH%
cmake -S . -B build-cli -G Ninja -DCMAKE_PREFIX_PATH=C:\Qt\6.12.0\mingw_64 -DCMAKE_BUILD_TYPE=Debug
cmake --build build-cli
```

`6.12.0`、`mingw_64`、`mingw1310_64` 必须和资源管理器 `C:\Qt` 里看到的一致。生成器是 **Ninja**，不是手写 Makefile；`cmake --build` 会去调 `ninja`。

---

## 9. Day 1 完成标准（打勾再进入 Day 2）

- [ ] Qt Creator 能打开，Kits 里有绿色/黄色的 **Qt 6 MinGW 64-bit**
- [ ] `CMakeLists.txt` 已 Configure 成功，无红字
- [ ] 点 Run 弹出 DeviceLink 主窗口，Port 为 **19000**
- [ ] （可选）`python --version` 有输出
- [ ] （加分）Connect `127.0.0.1:19000` + 发送 `hello` 表格出现一行

卡住时把这三样发出来即可：① 卡在第几步 ② Creator 底部英文报错全文 ③ Kits 页面截图描述（红还是绿）。
