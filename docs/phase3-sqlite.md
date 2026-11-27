# Phase 3：SQLite 帧历史（约 10 天，每天 30～45 分钟）

在 DeviceLink **已有 TCP + Serial + FrameParser** 上，加 **Qt Sql + SQLite**，把解析出的每一帧落盘，练 SQL、持久化和一点菜单/文件对话框。仍然是入门项目，不是数据库课。

**前置**：Phase 1～2 能跑通 TCP echo；CMake 会多链 `Qt6::Sql`。

---

## 学完能说什么

- 用 `QSqlDatabase::addDatabase("QSQLITE")` 打开本地 `.db`
- 建表、`QSqlQuery` 预编译 `INSERT` / `SELECT` / `DELETE`
- 收到完整帧后写库；可选启动时把最近 N 条载入表格
- 菜单 **Clear** 只清界面 vs 同时清库（自己定义）
- 面试：**「MySQL 换 `QMYSQL` + 连接串，查询写法同类。」**

---

## 表设计（全程用这一张即可）

```sql
CREATE TABLE IF NOT EXISTS frames (
    id          INTEGER PRIMARY KEY AUTOINCREMENT,
    ts          TEXT    NOT NULL,   -- 本地时间 "yyyy-MM-dd hh:mm:ss"
    channel     TEXT    NOT NULL,   -- "tcp" 或 "serial"
    len         INTEGER NOT NULL,
    payload     BLOB    NOT NULL,
    hex         TEXT    NOT NULL
);
```

- `payload` 用 `QByteArray` 绑定；`hex` 存 `toHex(' ')` 方便直接显示，不必每次重算。

数据库文件建议：`QStandardPaths::writableLocation(AppDataLocation) + "/devicelink.db"`（跨会话、少污染工程目录）。

---

## 推荐新增类（别全塞进 MainWindow）

| 类 | 职责 |
|----|------|
| `FrameStore` | `open()`、`insertFrame()`、`fetchRecent(n)`、`clearAll()`、`lastError()` |

`MainWindow` 里持有一个 `FrameStore m_store;`，构造末尾 `m_store.open()`。

---

## 第 1 周 — 能存、能查

### Day 1：环境与空库

- Maintenance Tool 确认已装 **Qt Sql**（通常随 Qt 一起；没有就 Add Components）。
- `CMakeLists.txt`：`find_package(... Sql)`，`target_link_libraries(... Qt6::Sql)`。
- `#include <QSqlDatabase>` 编译通过即可（先不写业务）。
- **完成标准**：Rebuild 成功。

### Day 2：`FrameStore::open` + 建表

- 新建 `FrameStore.h/.cpp`，加入 `CMakeLists.txt`。
- `open()`：`addDatabase("QSQLITE")`，`setDatabaseName(路径)`，`open()` 失败返回 `false` 并记 `lastError`。
- 执行上面的 `CREATE TABLE IF NOT EXISTS`。
- **完成标准**：Run 一次，磁盘上出现 `.db`（路径在 About 或日志里 `appendLog` 一行便于你找）。

### Day 3：`insertFrame`

- `insertFrame(channel, len, payload)`：预编译 `INSERT INTO frames ...`。
- 用 `QDateTime::currentDateTime().toString(Qt::ISODate)` 写 `ts`。
- `payload`：`query.addBindValue(payload)`；`hex`：`payload.toHex(' ')`。
- **完成标准**：单元可写个临时按钮或 Day 4 再接 UI；用 [DB Browser for SQLite](https://sqlitebrowser.org/) 打开 `.db` 看到一行。

### Day 4：接到 `addFrameRow`

- 在 `addFrameRow(const DeviceFrame &fr)` 末尾（或 `onBytes` 里 for 循环内）调用 `m_store.insertFrame(...)`。
- `channel` 用 `isTcpMode() ? "tcp" : "serial"`（**在插入那一刻** 记 channel；若已连接可再记 `m_activeChannel` 枚举更准，可选）。
- insert 失败：`appendLog("DB: " + m_store.lastError())`，不弹阻塞框。
- **完成标准**：echo 发 `hello`，表里多一行，表格仍正常。

### Day 5：错误与关闭

- `MainWindow::closeEvent` 里 `m_store.close()`（`QSqlDatabase::removeDatabase` 按 Qt 文档配对）。
- `open()` 失败时：日志警告，程序 **仍可无库运行**（插入时 if (!m_store.isOpen()) return）。
- **完成标准**：删 db 文件再 Run，自动建表；关窗无崩溃。

### Day 6：菜单「Clear database…」

- File 菜单加 **Clear frame history**（与 Clear log **分开**）。
- 确认对话框 `QMessageBox::question` 后 `clearAll()`：`DELETE FROM frames` 或 `DROP`+ 重建。
- **完成标准**：清库后 DB 无行，界面表格可仍保留（或提示用户自己 Clear log）。

### Day 7：周回顾

- TCP + Serial 各测 1 次，确认 channel 字段正确。
- README 里一句话：帧历史存 SQLite。
- **Git commit**：`phase3: sqlite frame store`

---

## 第 2 周 — 读回、导出、面试向

### Day 8：启动加载最近 N 条（可选但推荐）

- `fetchRecent(100)`：`SELECT ... ORDER BY id DESC LIMIT ?` 再反序显示，或 `ORDER BY id ASC LIMIT` 取最后 100。
- 在 `buildUi` 之后、`loadSettings` 附近：若表非空，填 `m_table`（注意别和实时 RX 行重复逻辑，可只加载历史、不清空实时）。
- **简化版**：仅 **菜单「Reload history」** 手动加载，不做自动（省半小时）。
- **完成标准**：关程序再开，Reload 后表格有旧数据。

### Day 9：导出 CSV

- File → **Export frames to CSV…**，`QFileDialog::getSaveFileName`。
- 写 UTF-8：`ts,channel,len,payload_utf8,hex` 或 payload 只写 hex。
- 从 `SELECT * FROM frames ORDER BY id` 读（不必经过 `QTableWidget`）。
- **完成标准**：Excel/WPS 能打开。

### Day 10：打磨 + 口述

- About 加一句：SQLite 存帧历史；路径简述。
- `docs/recap.md` 加 **QSql** 两问（见下）。
- About / README 诚实：**未做 MySQL、未做 Model/View 绑 SQL**。

---

## Phase 3 口述补两句

**为什么 SQLite 不是 MySQL？**  
本机单用户调试工具，零配置、单文件备份；Qt 换 `QMYSQL` 即可接远程库，SQL 与 `QSqlQuery` 用法一致。

**表为什么不用 QSqlTableModel？**  
表很小，手动 `INSERT` + 偶发 `SELECT` 更清晰；大数据量再 Model + `QTableView`。

---

## 不做（避免 scope 爆炸）

- 多表、用户登录、同步 MySQL
- QML 绑 SQL
- 每条 RX 原始字节都入库（只存 **完整帧**）
- 在 GUI 线程做百万行 `SELECT`（以后可加 LIMIT）

---

## 和路线图的关系

```text
Phase 1  Widgets + TCP + FrameParser     [done]
Phase 2  Serial + 双通道 UI             [done]
Phase 3  SQLite 帧历史                  [本文件，约 10 天]
Phase 4  选修：Charts / Qt Test / QML 小 demo
```

每日超时跳过即可；Phase 3 做完 DeviceLink 作为 **「Qt 入门 + 小全栈上位机」** 就足够收工。