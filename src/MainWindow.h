#pragma once

#include "FrameParser.h"
#include "LocalEchoServer.h"
#include "TcpSession.h"
#include "SerialSession.h"

#include <QMainWindow>

class QCloseEvent;

class QAction;
class QLabel;
class QLineEdit;
class QPlainTextEdit;
class QPushButton;
class QSpinBox;
class QTableWidget;
class QComboBox;

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
    void onChannelChanged(int index);
    void onRefreshPorts();

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
    void clearRxBytes();
    void updateRxStatus();
    void refreshSerialPorts();
    void updateChannelUi();

    TcpSession m_session;
    SerialSession m_serial;
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
    QLabel *m_rxLabel = nullptr;
    quint64 m_rxBytes = 0;

    QComboBox *m_channelCombo = nullptr;
    QComboBox *m_serialPortCombo = nullptr;
    QComboBox *m_baudCombo = nullptr;
    QPushButton *m_refreshPortBtn = nullptr;

    QWidget *m_tcpRowWidget = nullptr;
    QWidget *m_serialRowWidget = nullptr;

    QAction *m_connectAct = nullptr;
    QAction *m_disconnectAct = nullptr;
};
