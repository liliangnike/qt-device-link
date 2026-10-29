# Qt Widgets 核心（两周够用）

给 Qt 零基础、或只会 C / 命令行程序的人看。目标：对着这份文档能讲清、能改 DeviceLink，不必再去网上搜同类概念。QML、自定义 Model/View、串口本课不做。

每天先读对应节，再改代码（见仓库根目录 `PLAN.md`）。

| 计划 | 读本节 |
|---|---|
| Day 2 对象树 | §2 |
| Day 3 信号槽 | §3 |
| Day 4 布局 | §4 |
| Day 5 窗口/菜单 | §5 |
| Day 6 TCP | §6.1～6.3 |
| Day 7 日志 | §5.4 `QPlainTextEdit` |
| Day 8 粘包 | §6.4 |
| Day 9 表格 | §5.5 |
| Day 10 别卡界面 | §6.1、§6.5 |
| Day 11 设置 | §6.6 |
| Day 14 口述 | §8、§9、[`recap.md`](recap.md) |

本仓库文件对照：

| 文件 | 干什么 |
|---|---|
| `src/main.cpp` | 创建 `QApplication`、显示主窗口、进入事件循环 |
| `src/MainWindow.*` | 界面、菜单、把按钮/socket 接到槽 |
| `src/TcpSession.*` | 封装 `QTcpSocket`：连、发、收 |
| `src/LocalEchoServer.*` | 本机 `127.0.0.1:19000` echo |
| `src/FrameParser.*` | 长度前缀帧、处理粘包/半包 |
| `CMakeLists.txt` | 告诉 CMake 编哪些文件、链哪些 Qt 库 |

默认联调地址：**`127.0.0.1:19000`**。不要用 9000。

---

## 1. Qt 是什么

Qt 是一套 **C++ 应用框架**：窗口、按钮、网络、线程、字符串、容器都有现成类。写上位机时，你很少从 Win32 API 或原始 POSIX socket 开始，而是用 Qt 的类，让事件循环帮你“有数据了再回调”。

招聘里写 **Qt 开发**，默认多半是 **桌面 Widgets**（工控软件、设备调试工具），不是手机 App，也不是网页。

### 1.1 Widgets 和 QML 不要混

| | Qt Widgets | Qt Quick / QML |
|---|---|---|
| 怎么写界面 | C++ 里 `new QPushButton` | `.qml` 声明式（有点像带逻辑的 JSON/JS） |
| 长什么样 | 传统桌面：菜单、表格、对话框 | 动画、触屏、仪表盘更常见 |
| 本课 | **只用这个** | 两周不碰 |
| 口头怎么说 | 桌面调试工具主流还是 Widgets | “本课没做 QML，需要的话可以后补” |

DeviceLink 整窗都是 Widgets：`QMainWindow`、`QPushButton`、`QLineEdit`、`QPlainTextEdit`、`QTableWidget`。

### 1.2 几个词必须会念

- **模块**：Qt 拆成许多库，用哪个在 CMake **两处**写上，并且安装器里也要勾过。只改 CMake、硬盘上没装，会报 `Could not find Qt6Xxx`。

  本项目现在是 Widgets + Network：

  ```cmake
  find_package(Qt6 REQUIRED COMPONENTS Widgets Network)
  target_link_libraries(DeviceLink PRIVATE Qt6::Widgets Qt6::Network)
  ```

  若再用 SQL，两处都加上 `Sql` / `Qt6::Sql`。`Widgets` 会自动带上 **Gui** 和 **Core**，这两个不用自己写。

  模块很多，两周不必背全表。常见的如下（本课只用前两行加粗的）：

  | 模块 | CMake 名 | 干什么 |
  |---|---|---|
  | Core | （被别的带着） | `QObject`、`QString`、`QByteArray` |
  | Gui | （Widgets 会带） | 窗口、绘图底层 |
  | **Widgets** | `Qt6::Widgets` | 按钮、布局、本课界面 |
  | **Network** | `Qt6::Network` | `QTcpSocket` / `QTcpServer` |
  | Concurrent | `Qt6::Concurrent` | `QtConcurrent::run`，别卡界面 |
  | Sql | `Qt6::Sql` | 连 MySQL/SQLite |
  | SerialPort | `Qt6::SerialPort` | 串口；工控常见，需在安装器里另勾 |
  | PrintSupport | `Qt6::PrintSupport` | 打印 |
  | Svg | `Qt6::Svg` | SVG 图标 |
  | Multimedia | `Qt6::Multimedia` | 音视频 |
  | Charts | `Qt6::Charts` | 曲线图 |
  | Quick / Qml | `Qt6::Quick` | QML 界面，本课不做 |
  | Test | `Qt6::Test` | 单元测试 |

  口头总结：Qt 按模块拆库，用到网络就链 Network，用到数据库就链 Sql；DeviceLink 只链了 Widgets + Network。本课先不要加模块。真要用串口：Maintenance Tool → Add/Remove → 勾 **Qt SerialPort**，再改 CMake。

- **moc（Meta-Object Compiler）**：额外的代码生成器。类上写了 `Q_OBJECT` 才能用信号槽、`tr()`。编译前 moc 读头文件，生成 `moc_MainWindow.cpp` 这类文件再一起编。这不是标准 C++ 编译器自带的。
- **事件循环**：`QApplication::exec()` 里那个死循环。它等：鼠标、键盘、定时器、socket 可读。有事件就调用你的槽。嵌入式里类似 `select()`/`epoll` 循环，只不过 GUI 和网络都挂在同一条循环上。
- **Kit（构建套件）**：Qt Creator 里“用哪套编译器 + 哪份 Qt 库”。本课用 **MinGW 64-bit**，不要用没装 VS 的 MSVC Kit。

### 1.3 和命令行 / 嵌入式 C 对比

若你以前写过 Linux 下的 C 网络程序，可以这样对照（没写过可跳过）：

| 命令行 / 板端 | 这边 |
|---|---|
| 自己的 TCP 进程 | Windows 上 Qt 客户端连那个进程 |
| `Makefile` + `gcc` | CMake 生成 Ninja，再用 MinGW `g++` |
| `recv()` 循环里自己拼包 | `readyRead` 槽里把字节丢给 `FrameParser` |
| `printf` / 日志文件 | `QPlainTextEdit` 日志 |

Qt 5 和 Qt 6：本课是 **Qt 6**。老资料里的 `Qt5 Widgets`、`qmake`（`.pro` 文件）先当历史。Qt 6 官方工程用 CMake。

---

## 2. 对象树和内存

Qt 界面对象几乎都继承 **`QObject`**。每个 `QObject` 可以指定一个 **parent**。父对象析构时，会自动 `delete` 所有孩子。这叫 **对象树**，用来管界面控件的寿命，避免你在窗口关掉后还自己挨个 `delete`。

`QWidget`（能画在屏幕上的东西）也是 `QObject`，所以按钮、输入框同样走这套树。

### 2.1 `main.cpp` 在干什么

```cpp
int main(int argc, char *argv[])
{
    QApplication app(argc, argv);          // 必须先有，而且整个进程通常只有一个
    QApplication::setApplicationName("DeviceLink");
    QApplication::setOrganizationName("DeviceLink");

    MainWindow w;   // 栈上的主窗口，不是 new
    w.show();       // 不 show 就看不见
    return app.exec();  // 进入事件循环，直到窗口关掉
}
```

- `QApplication` 必须在创建任何窗口 **之前** 构造。`argc/argv` 要传进去（Qt 会认一些命令行参数）。
- `MainWindow w` 在栈上。`main` 结束时 `w` 析构，整棵窗口对象树一起拆。
- `app.exec()` **会阻塞**，直到退出事件循环（用户关窗口）。返回值当进程退出码。

`setApplicationName` / `setOrganizationName` 给 `QSettings` 用，决定设置写到注册表哪条路径（见 §6.6）。

### 2.2 构造函数里谁是谁的 parent

`MainWindow` 构造顺序：设标题和大小 → `buildUi()` → 菜单 → 信号槽 → 强制 `127.0.0.1:19000` → 开本机 echo。

`buildUi()` 里关键几行：

```cpp
auto *central = new QWidget(this);   // parent = 主窗口
setCentralWidget(central);           // 告诉 QMainWindow：中间那块是它

m_hostEdit = new QLineEdit(central);     // parent = central
m_connectBtn = new QPushButton(tr("Connect"), central);
```

对象树（简化）：

```text
MainWindow  (栈上的 w)
├── central QWidget          new QWidget(this)
│   ├── Host 标签、QLineEdit、QSpinBox、Connect/Disconnect 按钮
│   ├── 发送框、Send 按钮
│   └── QSplitter
│       ├── QPlainTextEdit 日志
│       └── QTableWidget
├── 菜单栏、状态栏（QMainWindow 自带）
│   └── QLabel 状态文字     new QLabel(..., this)
├── TcpSession m_session     成员变量，不是 new 出来的控件
├── LocalEchoServer m_echo   同上
└── FrameParser m_parser     普通 C++ 对象，不是 QObject
```

读 `new QXxx(central)` 时，括号里那个就是 parent。窗口关掉 → `MainWindow` 析构 → Qt 按对象树 `delete` 那些 `new` 出来的控件。你 **不必** 在析构函数里写 `delete m_hostEdit`。

### 2.3 成员变量 vs `new` 出来的控件

`MainWindow.h` 里有两类东西，不要混：

| 写法 | 例子 | 谁释放 |
|---|---|---|
| 值成员（不是指针） | `TcpSession m_session;` `LocalEchoServer m_echo;` | C++ 析构 `MainWindow` 时自动析构成员 |
| 指针 + `new`，并指定 parent | `m_hostEdit = new QLineEdit(central);` | parent 析构时 Qt 删除 |
| 指针 + `new`，**没有** parent | `new QXxx;` 忘写 parent | **你必须自己 `delete`**，否则泄漏 |

本项目控件都指定了 parent，所以没有在析构里 `delete` 按钮。

`TcpSession` 内部是 `QTcpSocket m_socket;`（也是值成员）。`TcpSession` 析构时 `m_socket` 一起析构，socket 会关掉。不必对 `m_socket` 再 `delete`。

### 2.4 布局对象也有所有权

```cpp
auto *connRow = new QHBoxLayout;          // 这里先没写 parent
...
auto *layout = new QVBoxLayout(central);  // parent = central
layout->addLayout(connRow);               // 垂直布局接管 connRow
```

`addLayout` / `addWidget` 之后，子布局、控件的 parent 会被 Qt 改到合适的 widget 上。记住一句即可：**布局负责几何（位置大小），对象树负责寿命。** 布局本身通常不显示成一块“看得见的控件”。

### 2.5 绝对不要做的事

1. **对已有 parent 的 `QObject` 再 `delete` 一遍**  
   窗口关时 Qt 会再删一次 → 二次释放，崩溃。  
   指针只当“借用”，寿命交给 parent。
2. **在槽里 `delete this` 或删正在发信号的对象**（进阶坑）。本课不要这么写。
3. **工作线程里 `new`/`delete` 界面控件**。控件必须在 GUI 线程创建、修改（见 §6.5）。

### 2.6 Day 2 要动手的两行

在 `MainWindow` 构造函数里：

```cpp
setWindowTitle(tr("DeviceLink"));  // Day 2：改成你的标题
resize(900, 600);                                     // 改成例如 1000, 700
```

`tr("...")` 是给翻译用的宏，本课没有做多语言，但习惯上界面字符串都包一层。改完 Rebuild、Run，看标题栏和窗口大小。

口述三句：

1. `central` 的 parent 是 `MainWindow`（`this`）。
2. 按钮的 parent 是 `central`。
3. 用户关窗口 → `w` 析构 → 对象树释放控件；`m_session` 作为成员随后析构。

---

## 3. 信号和槽（必考）

信号槽是 Qt 的“回调列表”，而且能跨对象、跨线程排队。

- **信号（signal）**：发生了什么。例如按钮被点了、TCP 连上了、socket 有数据了。你一般只 `emit`，不在信号函数里写业务。
- **槽（slot）**：你要干什么。例如 `onConnect()` 里去 `connectToHost`。
- **`connect`**：把二者接上。一个信号可以接多个槽；多个信号也可以接到同一个槽。

### 3.1 为什么类上要写 `Q_OBJECT`

两个类分工不同：网络类对外喊，窗口类主要听。窗口也可以自己发信号，本课用状态栏做了一条示例。

`MainWindow` 以槽为主，另外有一条自己的信号去更新状态栏：

```cpp
class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = nullptr);
signals:
    void statusMessage(const QString &text);  // 没有函数体，moc 生成
private slots:          // 点按钮、TCP 连上/收到数据时要做的事
    void onConnect();
    void onConnected();
    void onBytes(const QByteArray &data);
    ...
};
```

三条就齐：头文件声明 → `connect(this, &MainWindow::statusMessage, m_statusLabel, &QLabel::setText)` → `setConnectedUi` 里 `emit statusMessage(...)`。

`TcpSession` 要对窗口说“我连上了 / 我收到字节了”，所以有信号：

```cpp
class TcpSession : public QObject
{
    Q_OBJECT
signals:                // 对外通知，没有自己写的函数体
    void connected();
    void disconnected();
    void errorText(const QString &text);
    void bytesReceived(const QByteArray &data);
private slots:          // 听底层 QTcpSocket
    void onReadyRead();
    void onError(QAbstractSocket::SocketError socketError);
};
```

没有 `Q_OBJECT`：moc 不生成元对象代码，`connect` 新语法会编不过或运行期对不上。头文件必须进 `CMakeLists.txt` 的源列表，AUTOMOC 才会处理（见 §7）。

`signals:` / `slots:` 是 Qt 关键词，moc 会改写，不是标准 C++。一个类可以只有槽、只有信号，或两者都有。没有向外广播时，可以不写 `signals:`；加上也不难。

### 3.2 `connect` 怎么读（新语法，本项目全用这个）

```cpp
connect(发送者, &发送者类::信号, 接收者, &接收者类::槽);
```

本窗口里真实的几条：

```cpp
connect(m_connectBtn, &QPushButton::clicked,
        this, &MainWindow::onConnect);

connect(m_sendEdit, &QLineEdit::returnPressed,
        this, &MainWindow::onSend);

connect(&m_session, &TcpSession::connected,
        this, &MainWindow::onConnected);

connect(&m_session, &TcpSession::bytesReceived,
        this, &MainWindow::onBytes);
```

读法：Connect 按钮的 `clicked` → 调用 `MainWindow::onConnect`。  
`TcpSession` 发出 `bytesReceived` → `onBytes` 里写日志、喂给 `FrameParser`。

新语法用 **成员函数指针**，参数类型在 **编译期** 检查。老语法是字符串：`SIGNAL(clicked())`，打错字要运行才炸。口头说“用函数指针那种 connect”即可。

发送框回车和 Send 按钮接到 **同一个** `onSend`：一对多的反例（多信号一槽）。

### 3.3 `TcpSession` 里如何“转发”socket 信号

```cpp
connect(&m_socket, &QTcpSocket::connected, this, &TcpSession::connected);
connect(&m_socket, &QTcpSocket::readyRead, this, &TcpSession::onReadyRead);
```

`QTcpSocket::connected` 直接接到 `TcpSession::connected`。后者在头文件 `signals:` 里声明，没有自己写的函数体——moc 生成。这叫把底层信号 **再广播** 给 `MainWindow`，让窗口不必直接操作 socket。

有数据时：

```cpp
void TcpSession::onReadyRead()
{
    const QByteArray chunk = m_socket.readAll();
    if (!chunk.isEmpty())
        emit bytesReceived(chunk);   // 主动发射信号
}
```

`emit` 就是“调用所有已经 connect 到这个信号的槽”。本课槽和信号都在 GUI 线程，默认是 **直接调用**（DirectConnection），和普通函数调用很像。

### 3.4 跨线程时（先记结论，Day 10 用）

若发送者、接收者 **不在同一线程**，Qt 默认改成 **QueuedConnection**：把调用帖进接收者线程的事件循环，下次循环再执行槽。所以工作线程里不要直接改 `QLabel` 文字，应 `emit` 一个信号，让主窗口的槽去改。本工具没有工作线程，收包就在 GUI 线程。

### 3.5 和 C 函数指针回调的差别

| | C 回调 | Qt 信号槽 |
|---|---|---|
| 连接数量 | 常常一个函数指针 | 一对多 |
| 类型检查 | 靠你自己 | 新语法编译期 |
| 跨线程 | 自己加锁/队列 | 可自动排队到对象所在线程 |
| 依赖 | 标准 C | 要 moc、`QObject` |

### 3.6 Day 3 任务提示

给「Clear log」加工具栏：

```cpp
auto *tb = addToolBar(tr("Main"));
tb->addAction(tr("Clear log"), this, &MainWindow::onClearLog);
```

`QMainWindow::addToolBar` 得到 `QToolBar`。`addAction` 内部会 `connect` 到你给的槽。已有菜单项 `fileMenu->addAction(tr("Clear log"), this, &MainWindow::onClearLog)` 是同一槽，不必复制清理逻辑。

---

## 4. 布局（必会）

不要用 `setGeometry(x, y, w, h)` 把控件钉死在像素上。窗口一拉大，钉死的控件不动，会很难看。用 **布局管理器** 按方向排，拉窗口时按 stretch 分配多余空间。

### 4.1 三种常用布局

| 类 | 方向 | 本项目 |
|---|---|---|
| `QHBoxLayout` | 从左到右 | Host/Port/按钮那一行；发送框那一行 |
| `QVBoxLayout` | 从上到下 | 整个 `central`：连接行 → 发送行 → 分割器 |
| `QGridLayout` / `QFormLayout` | 格子 / 标签+控件 | Day 4 可把 Host/Port 改成表单 |

`addWidget(控件, stretch)` 的 **stretch** 是整数权重。`connRow->addWidget(m_hostEdit, 1)` 表示这一行多出来的宽度优先给 Host 输入框。按钮没写 stretch（默认 0），拉大窗口时按钮大致保持内容宽度。

`layout->addWidget(split, 1)`：垂直方向多出来的高度给分割器（日志+表），上面两行按钮区不跟着狂长。

### 4.2 `QMainWindow` 不能直接 `setLayout`

`QMainWindow` 已经内部分成：菜单栏、工具栏、**中心部件**、状态栏、停靠窗。你必须：

```cpp
auto *central = new QWidget(this);
setCentralWidget(central);
auto *layout = new QVBoxLayout(central);  // 布局打在 central 上，不是打在 MainWindow 上
```

这是新手最常见的坑：对 `QMainWindow` 直接 `new QVBoxLayout(this)`，菜单和中心区会乱。

### 4.3 `QSplitter`：可拖的分栏

```cpp
auto *split = new QSplitter(Qt::Vertical, central);
split->addWidget(m_log);
split->addWidget(m_table);
split->setStretchFactor(0, 2);  // 上面日志占 2
split->setStretchFactor(1, 1);  // 下面表占 1
```

用户可拖中间那条缝。这是布局之上又一层“用户可调比例”。

### 4.4 Day 4 怎么验证你懂了

改 `buildUi()` 里 Host/Port 行，例如改成 `QFormLayout`，或给某个 `addWidget` 改 stretch。Rebuild，**拉大、拉小窗口**，看谁跟着长、谁几乎不变。能讲清“stretch 和 splitter 各管什么”就算过关。

---

## 5. 常用控件（对着本项目说）

### 5.1 `QMainWindow`

带菜单、状态栏、中心区的顶层窗口。`MainWindow` **继承** 它，所以能 `menuBar()`、`statusBar()`、`setCentralWidget()`。

菜单：

```cpp
auto *fileMenu = menuBar()->addMenu(tr("&File"));  // &F 表示 Alt+F
fileMenu->addAction(tr("Connect"), this, &MainWindow::onConnect);
fileMenu->addSeparator();
fileMenu->addAction(tr("E&xit"), this, &QWidget::close);
```

`QAction` 是“可复用的命令”（菜单、工具栏、快捷键可共用同一个 Action）。`m_connectAct->setEnabled(false)` 可同时灰掉菜单项（`setConnectedUi` 里就是这么做的）。

Day 5：再加 `Help → Qt basics`，弹 `QMessageBox` 写三句（信号槽、parent、事件循环），或用 `QDesktopServices::openUrl` 打开本 md（打开文件是加分，弹框就够）。

### 5.2 输入：`QLineEdit` / `QSpinBox`

- `QLineEdit`：一行文本。`text()` 读取，`setText` 写入，`clear()` 清空，`setPlaceholderText` 是空时的灰字，`setEnabled(false)` 变灰。`returnPressed` 信号 = 用户按回车。
- `QSpinBox`：整数。`setRange(1, 65535)` 限制端口，`value()` / `setValue(19000)`。

连接成功后 `setConnectedUi(true)` 会禁止改 Host/Port，避免连着的时候改地址造成误会。

### 5.3 按钮：`QPushButton`

`clicked` 信号无参数。文本用 `tr("Connect")`。`setEnabled(false)` 变灰。

### 5.4 日志：`QPlainTextEdit`

多行纯文本，适合日志。本项目：

- `setReadOnly(true)`：用户不能当记事本改日志。
- `appendPlainText(...)`：末尾加一行。Day 7 可改成带 `[TX]`/`[RX]`，或 `appendHtml` 上色。
- `clear()`：Clear log 时连表一起清空。

和 `QTextEdit` 的差别：后者是富文本编辑器，日志用 Plain 更轻。

当前日志格式：`[时:分:秒.毫秒] 内容`，见 `appendLog()`。

### 5.5 表：`QTableWidget`（还不是 Model/View）

```cpp
m_table = new QTableWidget(0, 3, central);  // 0 行 3 列
m_table->setHorizontalHeaderLabels({tr("Time"), tr("Len"), tr("Payload")});
```

插入一行完整帧：

```cpp
const int row = m_table->rowCount();
m_table->insertRow(row);
m_table->setItem(row, 0, new QTableWidgetItem("12:00:00"));
m_table->setItem(row, 1, new QTableWidgetItem(QString::number(fr.length)));
m_table->setItem(row, 2, new QTableWidgetItem(QString::fromUtf8(fr.payload)));
```

`QTableWidgetItem` 的寿命由表管，不必自己 `delete`。

**常被问到：`QTableWidget` vs Model/View**

- `QTableWidget`：每个格子一个 item 对象，写起来快。行数到几千会吃内存、变慢。
- 正规做法：`QAbstractTableModel`（数据）+ `QTableView`（怎么画）。数据仍在你的 `QVector` 里，view 按需问模型“第 r 行第 c 列是什么”。

本课表很小，用 Widget 版。Day 9 加第 4 列 Hex：`QString::fromLatin1(fr.payload.toHex(' '))`。被问就按上面两句答，不要假装做过自定义 Model。

### 5.6 对话框：`QMessageBox`

```cpp
QMessageBox::about(this, tr("About DeviceLink"), tr("..."));
```

`this` 当父窗口，对话框是模态的（不关它不能点后面）。Day 12：Host 为空时 `QMessageBox::warning(this, tr("Connect"), tr("Host is empty"));`。还有 `information` / `critical` / `question`。

### 5.7 字符串和字节（改协议时会碰到）

| 类 | 用途 |
|---|---|
| `QString` | 界面文字，内部 UTF-16。`text.toUtf8()` 才能当 TCP 载荷 |
| `QByteArray` | 原始字节。socket 读写都用它 |
| `tr("Connect")` | 可翻译字符串；本课只有英文界面 |
| `QStringLiteral("127.0.0.1")` | 编译期造 `QString`，少一次运行时转换 |

不要把 `QString` 当 `char*` 直接 `write` 进 socket。本项目发送路径：`QString` → `toUtf8()` → 前面加 2 字节长度 → `QByteArray` → `m_socket.write`。

---

## 6. 事件循环和网络

### 6.1 事件循环（为什么不能 `sleep`）

`app.exec()` 里反复：

1. 等操作系统事件（鼠标、键盘、socket、定时器）。
2. 分发给对应对象。
3. 调用你的槽。
4. 槽返回后，再回去等。

槽 **必须尽快返回**。若在 `onConnect` 里 `while (!connected) sleep(1);`，事件循环转不动：窗口不能画、按钮不能点、socket 的“已连上”也送不进槽 → 假死。这和在嵌入式主循环里死等 `recv` 把整个设备卡住是同一类问题。

`QTcpSocket` 是 **异步** 的：`connectToHost` 立刻返回，真正连上之后才发 `connected` 信号。

### 6.2 本项目 TCP 客户端（`TcpSession`）

封装目的：`MainWindow` 只谈“连/断/收/发”，不直接碰 `QTcpSocket`。

```text
用户点 Connect
  → onConnect()
  → m_session.connectTo(host, port)
  → m_socket.connectToHost(...)     // 立即返回
  → （事件循环继续转）
  → 内核连上
  → QTcpSocket 发 connected
  → TcpSession 再发 connected
  → MainWindow::onConnected → 按钮变“已连接”
```

收数据：

```text
内核缓冲区有字节
  → readyRead
  → onReadyRead: readAll()
  → emit bytesReceived(chunk)
  → MainWindow::onBytes：写 RX 日志，m_parser.feed(chunk)
```

发送：`write` 把数据放进 Qt/内核缓冲区，`flush` 尽量立刻推出去（本课加了 flush，避免小包在 Windows 上坐一会儿）。`write` 成功不代表对端已经处理完。

`RemoteHostClosedError`：对端正常关连接时也会报这个“错误”。本课在 `onError` 里忽略它，避免日志里又 Error 又 Disconnected 吓到人。真正的失败（拒连、超时）仍 `emit errorText`。

状态：`m_socket.state() == QAbstractSocket::ConnectedState` 才允许 `sendBytes`。未连上就点 Send，函数直接 return。

### 6.3 本机 echo（`LocalEchoServer`）

`QTcpServer` 在 `127.0.0.1:19000` 上 `listen`。有客户端进来发 `newConnection`，取出 `QTcpSocket*`，有数据就按同一套长度前缀把整帧原样写回。

启动时 `MainWindow` 构造函数末尾调用 `onStartLocalEcho()`，所以你通常 **不必** 再跑 `echo_server.py`。Python 脚本是对照用的，端口同样 19000；不要和内置 echo **同时** 听这个口。

Windows 上 Python 的阻塞 `accept`/`recv` 可能导致 Ctrl+C 退不出，任务管理器结束 `python.exe`。内置 echo 走 Qt 事件循环，关窗口即可。

### 6.4 粘包 / 半包（`FrameParser`）

TCP 是字节流，**没有“一条消息”边界**。一次 `readAll()` 可能是：

- 刚好一帧；
- 两帧粘在一起；
- 只有 1 字节长度头（半包）；
- 上一帧的尾巴 + 下一帧的头。

所以应用层必须自己定边界。本课格式：

```text
偏移 0～1：uint16 大端（网络序）= payload 字节数
偏移 2 起：payload（本课是 UTF-8 文本）
```

例：文本 `hello` 是 5 字节，线上是：

```text
00 05  68 65 6c 6c 6f
```

`qFromBigEndian<quint16>` 把前两字节读成长度。缓冲不够 `2+len` 就 `break`，等下次 `feed`。超过 4096 或长度为 0 视为坏数据，丢掉 1 字节再试（简单防resync，不是工业级）。

`onConnect` 里 `m_parser.reset()`：旧连接半包不要带到新连接。

口头总结：和设备侧自己拼私有帧是同一类问题；上位机用长度前缀，设备侧常见长度前缀或分隔符。

Day 8：能讲清“为什么必须先收齐 2 字节再收 payload”。不必上 Wireshark。

### 6.5 会不会卡住界面？

本工具：`readyRead` → 解析 → `appendPlainText` / 插表，全在 GUI 线程。实验室小流量没问题。

大流量（高速采样、抓包）应：

- socket 或解析放到 `QThread`，`moveToThread`；
- 用信号把“已经解析好的一行”送回主线程更新表；
- 不要在工作线程调用 `m_log->appendPlainText`。

加分理解：小工具 GUI 线程收包可以；量上去要线程 + 信号排队。本仓库没做 `moveToThread`。

### 6.6 `QSettings`（记 host/port）

```cpp
QSettings s;
s.setValue("host", m_hostEdit->text());
s.setValue("port", m_portSpin->value());
```

Windows 上默认写注册表，路径大致：

`HKEY_CURRENT_USER\Software\DeviceLink\DeviceLink`

组织和应用名来自 `main.cpp` 的 `setOrganizationName` / `setApplicationName`。

**本课现状：** `loadSettings()` 为避免再连到旧的 9000，**每次启动都强制** `127.0.0.1:19000`。`saveSettings()` 仍会写注册表，但启动时不读回来。Day 11 若要“关掉再开还记住端口”，再改 `loadSettings` 去 `s.value("port", 19000)`；练习阶段请继续用 19000。

关窗口时 `closeEvent` 里调用 `saveSettings()`，再交给基类真正关闭。

---

## 7. CMake 要点

CMake 是 **元构建**：你写 `CMakeLists.txt`（要编谁、链谁），它生成 Ninja/Makefile，再由 Ninja/`make` 调 `g++`。嵌入式手写 Makefile 是直接指挥 `gcc`；桌面 Qt 用 CMake 是因为 moc、不同 Kit、Windows/Linux 差异太多（详见仓库里对 CMake vs `make` 的笔记，命令行编法见 `INSTALL-WINDOWS.md` 第 8 节）。

### 7.1 本项目 `CMakeLists.txt` 逐行

```cmake
cmake_minimum_required(VERSION 3.16)
project(DeviceLink VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)   # 自动对 Q_OBJECT 头文件跑 moc
set(CMAKE_AUTORCC ON)   # 有 .qrc 资源文件才需要；本课没有也无妨
set(CMAKE_AUTOUIC ON)   # 有 Qt Designer 的 .ui 才需要；本课界面全手写 C++

find_package(Qt6 REQUIRED COMPONENTS Widgets Network)

qt_add_executable(DeviceLink
    src/main.cpp
    src/MainWindow.h      # 头文件也要列出，AUTOMOC 才能找到 Q_OBJECT
    src/MainWindow.cpp
    ...
)

target_link_libraries(DeviceLink PRIVATE Qt6::Widgets Qt6::Network)
```

`WIN32_EXECUTABLE TRUE`：Windows 上当 GUI 程序，不弹黑色控制台窗口。

漏掉头文件的典型现象：信号槽编译报错，或链接时找不到 moc 生成的符号。

### 7.2 Creator 和命令行

Qt Creator 配置工程后，缓存不在 `build\` 根下，而在例如：

`build\Desktop_Qt_6_12_0_MinGW_64_bit_Debug\`

所以 `cmake --build build` 会报 `could not load cache`。应对那条子目录 `--build`，或另建 `build-cli`（路径以你 `C:\Qt\6.12.0\mingw_64` 为准）。日常点 Creator 绿色三角即可。

生成器是 **Ninja**，不是手写 Makefile。`cmake --build` = 调用 ninja。

### 7.3 改完代码怎么编

Creator：`Ctrl+B` 构建，`Ctrl+R` 运行。改了 `CMakeLists.txt` 才需要重新 Configure。不要手动去 `build` 文件夹双击 exe 当“官方运行方式”（缺 DLL 路径时可能弹不出）；用 Creator Run 会带上 Qt 的 `bin`。

---

## 8. 学完能讲清什么

**30 秒：** 用 Qt 6 Widgets 在 Windows 上做了一个 TCP 调试客户端。界面负责连接、发送、日志和解析表。协议是 2 字节大端长度加 payload，自己处理了 TCP 粘包。

和“设备上的程序”怎么对应（有板端经验可这样讲；没有就只讲客户端）：

| 设备 / 服务器侧 | DeviceLink |
|---|---|
| 板子上的协议栈、守护进程 | 电脑上的调试客户端 |
| `listen` / `accept` | 这里是 `QTcpSocket` 客户端 + 可选本机 echo |
| 半包、长度字段、网络字节序 | `FrameParser` + `uint16` BE |
| 串口或其它私有帧（本仓库没做串口） | 先把 TCP 和界面练会 |

本课没做：QML、自定义 Model、串口。`QSettings` 启动时仍强制 `127.0.0.1:19000`。需要存配置时，这种体量用 `QSettings` 或 SQLite 即可。

---

## 9. 常见提问（按完整句子准备）

**1. 信号槽和函数回调差在哪？**  
信号槽靠元对象，能一对多；新语法编译期检查类型；跨线程可以排队到接收者所在线程。普通函数指针一般一对一，跨线程要自己保证。

**2. Qt 里谁负责 `delete`？**  
有 parent 的 `QObject` 由父对象析构时删除。值成员走 C++ 析构。不要对有 parent 的对象再 `delete`。

**3. 为什么 GUI 不能在工作线程直接改控件？**  
控件不是线程安全的，必须在创建它的 GUI 线程改。工作线程 `emit` 信号，槽在主线程执行（QueuedConnection）。

**4. TCP 粘包怎么办？**  
TCP 是流。本项目用 2 字节大端长度前缀；`FrameParser` 缓冲，不够一帧就等。和设备上拼半包是同一类问题。

**5. Widgets 和 QML 怎么选？**  
传统桌面工具、菜单表格用 Widgets；触屏动效用 QML。本仓库是 Widgets。

**6. `QTableWidget` 和 Model/View？**  
表很小用 `QTableWidget`。大数据用 `QAbstractTableModel` + `QTableView`，避免每个格子一个 item。

**7. `connectToHost` 会不会阻塞界面？**  
不会一直卡死在函数里。它发起连接就返回；结果靠 `connected` / `errorOccurred`。不要在槽里 `sleep` 死等。

**8. 为什么要 moc？**  
标准 C++ 没有信号槽反射。moc 扫 `Q_OBJECT` 生成元对象代码。CMake `AUTOMOC` 自动做。头文件要进目标源列表。

**9. CMake 和 make 什么关系？**  
CMake 生成构建文件（这里是 Ninja）；Ninja/`make` 才真正调编译器。不是二选一。

**10. 本工具收包在哪条线程？**  
GUI 线程。数据量小可以。大流量应把 socket/解析 `moveToThread`，用信号更新界面。

---

改代码时把这份当词典：对象树看 §2，按钮点了没反应看 §3 的 `connect`，窗口拉不动看 §4，连上就断看端口 19000 和 §6。
