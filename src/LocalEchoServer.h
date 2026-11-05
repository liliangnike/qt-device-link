#pragma once

#include <QByteArray>
#include <QObject>
#include <QTcpServer>
#include <QTcpSocket>

class LocalEchoServer : public QObject
{
    Q_OBJECT

public:
    explicit LocalEchoServer(QObject *parent = nullptr);
    bool start(quint16 port);
    void stop();
    bool isListening() const;

signals:
    void logLine(const QString &text);

private slots:
    void onNewConnection();
    void onReadyRead();
    void onClientGone();

private:
    void feed(QTcpSocket *sock, const QByteArray &chunk);

    QTcpServer m_server;
    QByteArray m_buf;
};
