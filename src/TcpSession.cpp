#include "TcpSession.h"

TcpSession::TcpSession(QObject *parent)
    : QObject(parent)
{
    connect(&m_socket, &QTcpSocket::connected, this, &TcpSession::connected);
    connect(&m_socket, &QTcpSocket::disconnected, this, &TcpSession::disconnected);
    connect(&m_socket, &QTcpSocket::readyRead, this, &TcpSession::onReadyRead);
    connect(&m_socket, &QTcpSocket::errorOccurred, this, &TcpSession::onError);
}

bool TcpSession::isConnected() const
{
    return m_socket.state() == QAbstractSocket::ConnectedState;
}

void TcpSession::connectTo(const QString &host, quint16 port)
{
    if (m_socket.state() != QAbstractSocket::UnconnectedState)
        m_socket.abort();
    m_socket.connectToHost(host, port);
}

void TcpSession::disconnectFromHost()
{
    m_socket.disconnectFromHost();
}

void TcpSession::sendBytes(const QByteArray &data)
{
    if (!isConnected() || data.isEmpty())
        return;
    m_socket.write(data);
    m_socket.flush();
}

void TcpSession::onReadyRead()
{
    const QByteArray chunk = m_socket.readAll();
    if (!chunk.isEmpty())
        emit bytesReceived(chunk);
}

void TcpSession::onError(QAbstractSocket::SocketError socketError)
{
    if (socketError == QAbstractSocket::RemoteHostClosedError)
        return;
    emit errorText(m_socket.errorString());
}
