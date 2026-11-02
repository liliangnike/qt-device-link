#include "FrameParser.h"

#include <QtEndian>

void FrameParser::reset()
{
    m_buf.clear();
    m_ready.clear();
}

void FrameParser::feed(const QByteArray &chunk)
{
    m_buf.append(chunk);

    while (m_buf.size() >= 2) {
        const auto len = qFromBigEndian<quint16>(
            reinterpret_cast<const uchar *>(m_buf.constData()));
        if (len == 0 || len > 4096) {
            m_buf.remove(0, 1);
            continue;
        }
        if (m_buf.size() < 2 + len)
            break;

        DeviceFrame fr;
        fr.length = len;
        fr.payload = m_buf.mid(2, len);
        m_ready.push_back(fr);
        m_buf.remove(0, 2 + len);
    }
}

QVector<DeviceFrame> FrameParser::takeFrames()
{
    QVector<DeviceFrame> out = m_ready;
    m_ready.clear();
    return out;
}
