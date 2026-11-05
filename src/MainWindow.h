#pragma once

#include "FrameParser.h"
#include "LocalEchoServer.h"
#include "TcpSession.h"

#include <QMainWindow>

class QCloseEvent;

class QAction;
class QLabel;
class QLineEdit;
class QPlainTextEdit;
class QPushButton;
class QSpinBox;
class QTableWidget;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);

signals:
    void statusMessage(const QString &text);

private slots:
    void onConnect();
    void onDisconnect();
    void onSend();
    void onConnected();
    void onDisconnected();
    void onError(const QString &text);
    void onBytes(const QByteArray &data);
    void onClearLog();
    void onAbout();
    void onQtBasics();
    void onStartLocalEcho();

protected:
    void closeEvent(QCloseEvent *event) override;

private:
    static QColor getLogColor(const QString &line);
    void buildUi();
    void buildMenus();
    void loadSettings();
    void saveSettings();
    void setConnectedUi(bool on);
    void appendLog(const QString &line);
    void addFrameRow(const DeviceFrame &fr);

    TcpSession m_session;
    FrameParser m_parser;
    LocalEchoServer m_echo;

    QLineEdit *m_hostEdit = nullptr;
    QPushButton *m_echoBtn = nullptr;
    QSpinBox *m_portSpin = nullptr;
    QPushButton *m_connectBtn = nullptr;
    QPushButton *m_disconnectBtn = nullptr;
    QLineEdit *m_sendEdit = nullptr;
    QPushButton *m_sendBtn = nullptr;
    QPlainTextEdit *m_log = nullptr;
    QTableWidget *m_table = nullptr;
    QLabel *m_statusLabel = nullptr;

    QAction *m_connectAct = nullptr;
    QAction *m_disconnectAct = nullptr;
};
