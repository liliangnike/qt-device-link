#ifndef SERIALSESSION_H
#define SERIALSESSION_H

#include <QObject>
#include <QSerialPort>

class SerialSession : public QObject
{
    Q_OBJECT

public:
    explicit SerialSession(QObject *parent = nullptr);

    bool isConnected() const;
    void openPort(const QString& portName, qint32 baudRate);
    void closePort();
    void sendBytes(const QByteArray& data);

signals:
    void connected();
    void disconnected();
    void bytesReceived(const QByteArray& data);
    void errorText(const QString& text);

private slots:
    void onReadyRead();
    void onError(QSerialPort::SerialPortError error);

private:
    QSerialPort m_serial_port;
};

#endif // SERIALSESSION_H
