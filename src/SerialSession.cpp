#include "SerialSession.h"

SerialSession::SerialSession(QObject *parent)
    : QObject(parent)
{
    connect(&m_serial_port, &QSerialPort::readyRead, this, &SerialSession::onReadyRead);
    connect(&m_serial_port, &QSerialPort::errorOccurred, this, &SerialSession::onError);
}

void SerialSession::openPort(const QString& portName, qint32 baudRate)
{
    emit connected();
}

void SerialSession::closePort()
{
    emit disconnected();
}

void SerialSession::onReadyRead()
{
    const QByteArray chunk = m_serial_port.readAll();
    if(!chunk.isEmpty())
        emit bytesReceived(chunk);
}

void SerialSession::onError(QSerialPort::SerialPortError error)
{
    emit errorText(m_serial_port.errorString());
}