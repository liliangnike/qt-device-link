#include "SerialSession.h"

SerialSession::SerialSession(QObject *parent)
    : QObject(parent)
{
    connect(&m_serial_port, &QSerialPort::readyRead, this, &SerialSession::onReadyRead);
    connect(&m_serial_port, &QSerialPort::errorOccurred, this, &SerialSession::onError);
}

bool SerialSession::isConnected() const
{
    return m_serial_port.isOpen();
}

void SerialSession::openPort(const QString& portName, qint32 baudRate)
{
    if(isConnected()) closePort();
    m_serial_port.setPortName(portName);
    m_serial_port.setBaudRate(baudRate);
    if(!m_serial_port.open(QIODevice::ReadWrite)) {
        emit errorText(m_serial_port.errorString());
        return;
    }
    emit connected();
}

void SerialSession::closePort()
{
    if(!isConnected()) return;
    m_serial_port.close();
    emit disconnected();
}

void SerialSession::sendBytes(const QByteArray& data)
{
    if(!isConnected() || data.isEmpty())
        return;
    m_serial_port.write(data);
    m_serial_port.flush();
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