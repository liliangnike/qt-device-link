#include "MainWindow.h"

#include <QAction>
#include <QApplication>
#include <QCloseEvent>
#include <QDateTime>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QLabel>
#include <QLineEdit>
#include <QMenuBar>
#include <QMessageBox>
#include <QPlainTextEdit>
#include <QPushButton>
#include <QSettings>
#include <QSpinBox>
#include <QSplitter>
#include <QStatusBar>
#include <QTableWidget>
#include <QToolBar>
#include <QVBoxLayout>
#include <QColor>           // Color name
#include <QTextCharFormat>  // Characters format (color, bold, size, .etc)
#include <QTextCursor>      // Cursor in QPlainTextEdit

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    setWindowTitle(tr("DeviceLink — My Qt Lab"));
    resize(900, 600);

    buildUi();
    buildMenus();
    loadSettings();

    connect(this, &MainWindow::statusMessage, m_statusLabel, &QLabel::setText);
    setConnectedUi(false);

    connect(&m_session, &TcpSession::connected, this, &MainWindow::onConnected);
    connect(&m_session, &TcpSession::disconnected, this, &MainWindow::onDisconnected);
    connect(&m_session, &TcpSession::errorText, this, &MainWindow::onError);
    connect(&m_session, &TcpSession::bytesReceived, this, &MainWindow::onBytes);
    connect(&m_echo, &LocalEchoServer::logLine, this, &MainWindow::appendLog);

    m_hostEdit->setText(QStringLiteral("127.0.0.1"));
    m_portSpin->setValue(19000);
    onStartLocalEcho();
}

QColor MainWindow::getLogColor(const QString &line)
{
    if (line.startsWith(QStringLiteral("TX")))
        return QColor("#2e7d32");
    else if(line.startsWith(QStringLiteral("RX")))
        return QColor("#1565c0");
    else if(line.startsWith(QStringLiteral("Error")))
        return QColor("#c62828");
    else if(line == tr("Connected") || line == tr("Disconnected"))
        return QColor("#616161");

    return QColor("#202124");
}

void MainWindow::buildUi()
{
    auto *central = new QWidget(this);
    setCentralWidget(central);

    m_hostEdit = new QLineEdit(central);
    m_hostEdit->setPlaceholderText(tr("127.0.0.1"));
    m_portSpin = new QSpinBox(central);
    m_portSpin->setRange(1, 65535);
    m_portSpin->setValue(19000);
    m_connectBtn = new QPushButton(tr("Connect"), central);
    m_disconnectBtn = new QPushButton(tr("Disconnect"), central);
    m_echoBtn = new QPushButton(tr("Start local echo"), central);

    auto *connRow = new QHBoxLayout;
    connRow->addWidget(new QLabel(tr("Host"), central));
    connRow->addWidget(m_hostEdit, 1);
    connRow->addWidget(new QLabel(tr("Port"), central));
    connRow->addWidget(m_portSpin);
    connRow->addWidget(m_connectBtn);
    connRow->addWidget(m_disconnectBtn);
    connRow->addWidget(m_echoBtn);

    m_sendEdit = new QLineEdit(central);
    m_sendEdit->setPlaceholderText(tr("Text to send (Enter)"));
    m_sendBtn = new QPushButton(tr("Send"), central);
    auto *sendRow = new QHBoxLayout;
    sendRow->addWidget(m_sendEdit, 1);
    sendRow->addWidget(m_sendBtn);

    m_log = new QPlainTextEdit(central);
    m_log->setReadOnly(true);
    m_log->setPlaceholderText(tr("Raw RX/TX log"));

    m_table = new QTableWidget(0, 3, central);
    m_table->setHorizontalHeaderLabels({tr("Time"), tr("Len"), tr("Payload")});
    m_table->horizontalHeader()->setStretchLastSection(true);
    m_table->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_table->setEditTriggers(QAbstractItemView::NoEditTriggers);

    auto *split = new QSplitter(Qt::Vertical, central);
    split->addWidget(m_log);
    split->addWidget(m_table);
    split->setStretchFactor(0, 2);
    split->setStretchFactor(1, 1);

    auto *layout = new QVBoxLayout(central);
    layout->addLayout(connRow);
    layout->addLayout(sendRow);
    layout->addWidget(split, 1);

    m_statusLabel = new QLabel(tr("Disconnected"), this);
    statusBar()->addWidget(m_statusLabel);

    connect(m_connectBtn, &QPushButton::clicked, this, &MainWindow::onConnect);
    connect(m_disconnectBtn, &QPushButton::clicked, this, &MainWindow::onDisconnect);
    connect(m_sendBtn, &QPushButton::clicked, this, &MainWindow::onSend);
    connect(m_sendEdit, &QLineEdit::returnPressed, this, &MainWindow::onSend);
    connect(m_echoBtn, &QPushButton::clicked, this, &MainWindow::onStartLocalEcho);
}

void MainWindow::buildMenus()
{
    auto *fileMenu = menuBar()->addMenu(tr("&File"));
    m_connectAct = fileMenu->addAction(tr("Connect"), this, &MainWindow::onConnect);
    fileMenu->addSeparator();
    m_disconnectAct = fileMenu->addAction(tr("Disconnect"), this, &MainWindow::onDisconnect);
    fileMenu->addSeparator();
    fileMenu->addAction(tr("Clear log"), this, &MainWindow::onClearLog);
    fileMenu->addSeparator();
    fileMenu->addAction(tr("E&xit"), this, &QWidget::close);

    auto *tb = addToolBar(tr("Main"));
    tb->addAction(tr("Clear log"), this, &MainWindow::onClearLog);

    auto *helpMenu = menuBar()->addMenu(tr("&Help"));
    helpMenu->addAction(tr("About"), this, &MainWindow::onAbout);
    helpMenu->addAction(tr("Qt basics"), this, &MainWindow::onQtBasics);
}

void MainWindow::loadSettings()
{
    m_hostEdit->setText(QStringLiteral("127.0.0.1"));
    m_portSpin->setValue(19000);
}

void MainWindow::saveSettings()
{
    QSettings s;
    s.setValue("host", m_hostEdit->text());
    s.setValue("port", m_portSpin->value());
}

void MainWindow::setConnectedUi(bool on)
{
    m_connectBtn->setEnabled(!on);
    m_disconnectBtn->setEnabled(on);
    m_sendBtn->setEnabled(on);
    m_sendEdit->setEnabled(on);
    m_hostEdit->setEnabled(!on);
    m_portSpin->setEnabled(!on);
    if (m_connectAct)
        m_connectAct->setEnabled(!on);
    if (m_disconnectAct)
        m_disconnectAct->setEnabled(on);
    emit statusMessage(on ? tr("Connected") : tr("Disconnected"));
}

void MainWindow::appendLog(const QString &line)
{
    const QString ts = QDateTime::currentDateTime().toString("HH:mm:ss.zzz");
    const QString fullLine = QStringLiteral("[%1] %2").arg(ts, line);

    // comment out the original log print
    // m_log->appendPlainText(QStringLiteral("[%1] %2").arg(ts, line));

    const QColor color = getLogColor(line);

    // bind cursor to the internal log document
    QTextCursor cursor(m_log->document());

    // Move the cursor to the end of the doc
    cursor.movePosition(QTextCursor::End);

    // if the doc is not empty, enter a new line
    if(!m_log->document()->isEmpty()) {
        cursor.insertBlock();
    }

    // use which color for the line
    QTextCharFormat fmt;
    fmt.setForeground(color);

    // insert the line
    cursor.insertText(fullLine, fmt);

    // Update cursor and make sure it is visible in the latest line
    m_log->setTextCursor(cursor);
    m_log->ensureCursorVisible();

}

void MainWindow::addFrameRow(const DeviceFrame &fr)
{
    const int row = m_table->rowCount();
    m_table->insertRow(row);
    m_table->setItem(row, 0, new QTableWidgetItem(
        QDateTime::currentDateTime().toString("HH:mm:ss")));
    m_table->setItem(row, 1, new QTableWidgetItem(QString::number(fr.length)));
    m_table->setItem(row, 2, new QTableWidgetItem(QString::fromUtf8(fr.payload)));
    m_table->scrollToBottom();
}

void MainWindow::onConnect()
{
    const QString host = m_hostEdit->text();
    if(host.trimmed().isEmpty()) {
        QMessageBox::warning(this, tr("Connect"), tr("Host IP is empty!"));
        return;
    }

    saveSettings();
    m_parser.reset();
    appendLog(tr("Connecting to %1:%2 ...")
                  .arg(host)
                  .arg(m_portSpin->value()));

    // QString function trimmed is same as Python string strip()
    m_session.connectTo(host.trimmed(),
                        static_cast<quint16>(m_portSpin->value()));
}

void MainWindow::onDisconnect()
{
    m_session.disconnectFromHost();
}

void MainWindow::onSend()
{
    const QString text = m_sendEdit->text();
    if (text.isEmpty())
        return;
    QByteArray payload = text.toUtf8();
    QByteArray frame;
    const quint16 len = static_cast<quint16>(payload.size());
    frame.append(char((len >> 8) & 0xFF));
    frame.append(char(len & 0xFF));
    frame.append(payload);
    m_session.sendBytes(frame);
    appendLog(tr("TX  len=%1  %2").arg(len).arg(text));
    m_sendEdit->clear();
}

void MainWindow::onConnected()
{
    setConnectedUi(true);
    appendLog(tr("Connected"));
}

void MainWindow::onDisconnected()
{
    setConnectedUi(false);
    appendLog(tr("Disconnected"));
}

void MainWindow::onError(const QString &text)
{
    appendLog(tr("Error: %1").arg(text));
}

void MainWindow::onBytes(const QByteArray &data)
{
    appendLog(tr("RX  %1 bytes  %2")
                  .arg(data.size())
                  .arg(QString::fromUtf8(data.toHex(' '))));
    m_parser.feed(data);
    const auto frames = m_parser.takeFrames();
    for (const auto &fr : frames)
        addFrameRow(fr);
}

void MainWindow::onStartLocalEcho()
{
    const auto port = static_cast<quint16>(m_portSpin->value());
    if (m_echo.start(port)) {
        m_hostEdit->setText(QStringLiteral("127.0.0.1"));
        m_echoBtn->setEnabled(false);
    }
}

void MainWindow::onClearLog()
{
    m_log->clear();
    m_table->setRowCount(0);
}

void MainWindow::onAbout()
{
    QMessageBox::about(
        this, tr("About DeviceLink"),
        tr("A two-week Qt Widgets lab for beginners.\n"
           "TCP client + length-prefixed frames (uint16 BE + payload).\n"
           "Connect to 127.0.0.1:19000 (in-app local echo)."));
}

void MainWindow::onQtBasics()
{
    QMessageBox::about(
        this,
        tr("Qt Basics"),
        tr("Qt Modules: Widgets, Network, Sql, Concurrent, Multimedia, Charts, .etc\n"
           "Signal: trigger event\n"
           "Slots: action executor."));
}
void MainWindow::closeEvent(QCloseEvent *event)
{
    saveSettings();
    QMainWindow::closeEvent(event);
}
