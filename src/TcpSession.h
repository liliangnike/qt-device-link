#pragma once

#include <QObject>
#include <QTcpSocket>

class TcpSession : public QObject
{
    Q_OBJECT

public:
    explicit TcpSession(QObject *parent = nullptr);

    bool isConnected() const;
    void connectTo(const QString &host, quint16 port);
    void disconnectFromHost();
    void sendBytes(const QByteArray &data);

signals:
    void connected();
    void disconnected();
    void errorText(const QString &text);
    void bytesReceived(const QByteArray &data);

private slots:
    void onReadyRead();
    void onError(QAbstractSocket::SocketError socketError);

private:
    QTcpSocket m_socket;
};
