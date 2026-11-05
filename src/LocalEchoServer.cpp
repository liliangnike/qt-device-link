#include "LocalEchoServer.h"

#include <QHostAddress>
#include <QtEndian>

LocalEchoServer::LocalEchoServer(QObject *parent)
    : QObject(parent)
{
    connect(&m_server, &QTcpServer::newConnection,
            this, &LocalEchoServer::onNewConnection);
}

bool LocalEchoServer::start(quint16 port)
{
    stop();
    if (!m_server.listen(QHostAddress::LocalHost, port)) {
        emit logLine(tr("Local echo failed: %1").arg(m_server.errorString()));
        return false;
    }
    emit logLine(tr("Local echo listening on 127.0.0.1:%1").arg(port));
    return true;
}

void LocalEchoServer::stop()
{
    m_server.close();
    m_buf.clear();
}

bool LocalEchoServer::isListening() const
{
    return m_server.isListening();
}

void LocalEchoServer::onNewConnection()
{
    auto *sock = m_server.nextPendingConnection();
    if (!sock)
        return;
    m_buf.clear();
    connect(sock, &QTcpSocket::readyRead, this, &LocalEchoServer::onReadyRead);
    connect(sock, &QTcpSocket::disconnected, this, &LocalEchoServer::onClientGone);
    connect(sock, &QTcpSocket::disconnected, sock, &QObject::deleteLater);
    emit logLine(tr("Local echo: client connected"));
}

void LocalEchoServer::onReadyRead()
{
    auto *sock = qobject_cast<QTcpSocket *>(sender());
    if (!sock)
        return;
    feed(sock, sock->readAll());
}

void LocalEchoServer::onClientGone()
{
    m_buf.clear();
    emit logLine(tr("Local echo: client gone"));
}

void LocalEchoServer::feed(QTcpSocket *sock, const QByteArray &chunk)
{
    m_buf.append(chunk);
    while (m_buf.size() >= 2) {
        const auto len = qFromBigEndian<quint16>(
            reinterpret_cast<const uchar *>(m_buf.constData()));
        if (len == 0 || len > 4096) {
            m_buf.remove(0, 1);
            continue;
        }
        if (m_buf.size() < 2 + static_cast<int>(len))
            break;
        const QByteArray frame = m_buf.left(2 + len);
        m_buf.remove(0, 2 + len);
        sock->write(frame);
        emit logLine(tr("Local echo: bounced %1 bytes").arg(len));
    }
}
