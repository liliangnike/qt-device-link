#pragma once

#include <QByteArray>
#include <QVector>

struct DeviceFrame {
    quint16 length = 0;
    QByteArray payload;
};

class FrameParser
{
public:
    void reset();
    void feed(const QByteArray &chunk);
    QVector<DeviceFrame> takeFrames();

private:
    QByteArray m_buf;
    QVector<DeviceFrame> m_ready;
};
